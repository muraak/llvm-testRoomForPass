# LLVMのPASSのテストプロジェクト

## 単一の関数のみからアクセスされるグローバル変数を見つけてローカル化する

### 準備
---

#### テスト対象のCプログラムの準備

clangで関数をインライン化するためには，関数実体定義前に`__attribute__((always_inline))`を追加する。

```C
__attribute__((always_inline))
void incA() {
    gA++;
}
```

実験では，以下のファイルを構成する：

- `main.c`: エントリポイント。メイン関数にて，外部ファイルに定義されたインライン関数`incA()`を呼び出す。

- `ope.c`: インライン関数`incA()`を定義。 関数内で`main.c`で宣言しているグローバル変数`gA`にアクセスする。

- `ope.h`: `main.c`で`incA()`を参照するためのヘッダ。

#### リンク・インライン済みのllvmビットコードを生成する

すこしやっかいで以下のコマンド手順が必要：
（最適化レベルを1にしないと全部インライン化されたり全部されなかったりする)

- `clang -c -emit-llvm main.c ope.c -O1`

- `llvm-link main.bc ope.bc -o out.bc`

- `opt out.bc -o out.bc -O1` <==ここで外部ファイルのインライン関数が展開される


最後に生成した`out.bc`が今回の実験のインプットです！

### パスの作成
---

#### アルゴリズム

以下の手順でグローバル変数をローカル化する。

※全ファイルリンク済みかつインライン済みのIRがインプットであることが前提！

```

FOREACH G in モジュール内のグローバル変数
    FOREACH User in グローバル変数GのすべてのUser
        
        CAN_LOCALIZE := true // Gがローカル化できるかどうか
        
        F := Userが所属する関数を代入

        IF Fがインライン関数でない　THEN
            IF 初回 THEN
                F_REFS_G := F
            ELSE
                IF(F_REFS_G == F) THEN
                    CONTINUE
                ELSE
                    CAN_LOCALIZE = FALSE
                    BREAK
                END_IF
            END_IF
        END_IF
    END_FOREACH

    // グローバル変数を関数F_REFS_Gのローカル変数にする(F_REFS_G以外から参照されていないので)
    IF CAN_LOCALIZE == true THEN
        モジュールからグローバル変数Gを削除(G_oldに退避)
        関数F_REFS_G内に変数Gを追加(G_oldと同じ型情報)
        G_oldのusersに対し，G_oldへのUseをGへのUseに更新
    END_IF
END_FOREACH

```

#### 計算量

グローバル変数の数$N$，各グローバル変数の参照箇所数$M_i$に依存して$O(2N\overline{M})$となる。

### 結果

グローバル変数`@gA`はインライン化により`main()`のみから呼び出されているので
ローカル変数`%t`に置き換わっている！

- 適用前

```
define i32 @main() local_unnamed_addr #0 {
  %1 = load i32, i32* @gA, align 4, !tbaa !3
  %2 = add nsw i32 %1, 1
  store i32 %2, i32* @gA, align 4, !tbaa !3
  ret i32 0
}
```

- 適用後

```
define i32 @main() local_unnamed_addr #0 {
  %t = alloca i32*
  %1 = load i32, i32** %t, align 4, !tbaa !3
  %2 = add nsw i32 %1, 1
  store i32 %2, i32** %t, align 4, !tbaa !3
  ret i32 0
}
```

## 備考

- Macだとdump()とかllvmライブラリの出力系関数がコンパイルエラーになる。なんで？

- 関数の先頭に命令を追加したい時の方法がいまいちよくわからん。

Happy llvm!<br/>
Muraak.

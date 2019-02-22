# LLVMのPASSのテストプロジェクト

## 単一の関数にインライン化されることでローカル変数化できるグローバル変数を見つけてローカル化する

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

- `clang -c -emit-llvm main.c ope.c`

- `llvm-link main.bc ope.bc -o out.bc`

- `opt out.bc -o out.bc -O3` <==ここで外部ファイルのインライン関数が展開される


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
        
        F := Userをトラバースして所属する関数を代入()

        IF Fがインライン関数でない　THEN
            IF 初回 THEN
                G_REFBY := F
            ELSE
                IF(G_REFBY == F) THEN
                    CONTINUE
                ELSE
                    CAN_LOCALIZE = FALSE
                    BREAK
                END_IF
            END_IF
        END_IF
    END_FOREACH

    // グローバル変数を関数G_REFBYのローカル変数にする(G_REFBY以外から参照されていないので)
    IF CAN_LOCALIZE == true THEN
        モジュールからグローバル変数Gを削除(G_oldに退避)
        関数G_REFBY内に変数Gを追加(G_oldと同じ型情報)
        G_oldのusersに対し，G_oldへのUseをGへのUseに更新
    END_IF
END_FOREACH

```

#### 計算量

グローバル変数の数$N$，各グローバル変数の参照箇所数$M_i$に依存して$O(2N\overline{M})$となる。



Happy llvm!<br/>
Muraak.
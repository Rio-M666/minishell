# minishell - 完全処理フロー解説書

## 📋 目次

1. [全体の処理の流れ](#全体の処理の流れ)
2. [機能別の詳細解説](#機能別の詳細解説)
3. [全関数のI/O一覧](#全関数のio一覧)
4. [データフロー図](#データフロー図)

---

## 🌊 全体の処理の流れ

### エントリーポイント: [main.c:46-68](main.c#L46-L68)

```
┌─────────────────────────────────────────────────────────────┐
│ main()                                                      │
│ - シェル初期化 (envp保存, last_status=0)                    │
│ - シグナルハンドラ設定                                        │
└─────────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────────┐
│ while (1) ← REPLループ                                      │
│   ├─ readline("minishell> ") ... ユーザー入力待機           │
│   ├─ process_input(line, &shell) ... 入力処理               │
│   └─ free(line)                                             │
└─────────────────────────────────────────────────────────────┘
```

### process_input() の内部処理: [main.c:5-43](main.c#L5-L43)

```
┌────────────────────────────────────────────────────────────┐
│ [1] TOKENIZATION (字句解析)                                │
│     tokens = tokenize(input)                              │
│     入力文字列 → トークンのリンクリスト                      │
└────────────────────────────────────────────────────────────┘
                          ↓
┌────────────────────────────────────────────────────────────┐
│ [2] VARIABLE EXPANSION (変数展開)                          │
│     expand_tokens(tokens, shell)                          │
│     $VAR, $? などを環境変数値に置換                         │
└────────────────────────────────────────────────────────────┘
                          ↓
┌────────────────────────────────────────────────────────────┐
│ [3] PARSING (構文解析)                                      │
│     pipeline = parse(tokens)                              │
│     トークン → コマンドパイプライン構造                       │
└────────────────────────────────────────────────────────────┘
                          ↓
┌────────────────────────────────────────────────────────────┐
│ [4] HEREDOC PROCESSING (ヒアドック処理)                     │
│     process_heredocs(pipeline, shell)                     │
│     << で指定された内容を一時ファイルに読み込み              │
└────────────────────────────────────────────────────────────┘
                          ↓
┌────────────────────────────────────────────────────────────┐
│ [5] EXECUTION (コマンド実行)                                │
│     shell->last_status = execute_pipeline(pipeline, shell)│
│     fork/pipe/execve でコマンドを実行                       │
└────────────────────────────────────────────────────────────┘
                          ↓
┌────────────────────────────────────────────────────────────┐
│ [6] CLEANUP (後処理)                                        │
│     cleanup_heredocs(pipeline)                            │
│     free_pipeline(pipeline)                               │
└────────────────────────────────────────────────────────────┘
```

---

## 🔧 機能別の詳細解説

### 【機能1】TOKENIZATION (字句解析)

#### 処理フロー

```
input: "ls -la | grep test > out.txt"
  ↓
tokenize() [tokenizer/tokenizer.c:3-46]
  ↓
┌─ skip_space() ... 空白をスキップ
├─ is_special() ... 特殊文字チェック (|, <, >, <<)
│   YES → tokenize_special() ... 特殊文字トークン作成
│   NO  → get_word() ... 単語を抽出
│         ├─ get_quote_type() ... クォート種別判定
│         └─ strip_quotes() ... クォート削除
├─ create_token() ... トークン生成
└─ add_token() ... リストに追加
  ↓
output: トークンリンクリスト
[TOKEN_WORD("ls")] → [TOKEN_WORD("-la")] → [TOKEN_PIPE]
→ [TOKEN_WORD("grep")] → [TOKEN_WORD("test")]
→ [TOKEN_REDIR_OUT] → [TOKEN_WORD("out.txt")] → NULL
```

#### 使用関数の詳細

| 関数名 | 場所 | Input | Output | 処理内容 |
|--------|------|-------|--------|----------|
| `tokenize()` | [tokenizer/tokenizer.c:3-46](tokenizer/tokenizer.c#L3-L46) | `char *input` | `t_token *` | メイン処理。入力を走査してトークンリストを構築 |
| `skip_space()` | tokenizer_parse_utils.c | `char **str` | void | 空白文字をスキップし、ポインタを進める |
| `is_special()` | tokenizer_parse_utils.c | `char c` | `int` (bool) | 特殊文字(\|, <, >)なら1を返す |
| `tokenize_special()` | tokenizer_parse_utils.c | `char **str` | `t_token *` | \|, <, >, <<, >>をトークン化 |
| `get_word()` | tokenizer_parse_utils.c | `char **str` | `char *` | 次の単語を抽出（クォート対応） |
| `get_quote_type()` | [tokenizer_quote_utils.c](tokenizer/tokenizer_quote_utils.c) | `char *raw` | `t_quote_type` | 単一/二重クォートを判定 |
| `strip_quotes()` | [tokenizer_quote_utils.c](tokenizer/tokenizer_quote_utils.c) | `char *raw` | `char *` | クォート文字を削除した文字列を返す |
| `create_token()` | tokenizer_token_utils.c | `t_token_type type, char *value` | `t_token *` | 新しいトークン構造体を作成 |
| `add_token()` | tokenizer_token_utils.c | `t_token **head, t_token *new` | void | リストの末尾にトークンを追加 |

---

### 【機能2】VARIABLE EXPANSION (変数展開)

#### 処理フロー

```
tokens: [TOKEN_WORD("echo")] → [TOKEN_WORD("$USER")] → [TOKEN_WORD("$?")]
  ↓
expand_tokens() [expander/expander.c:15-39]
  ↓
各トークンに対して:
  - TOKEN_WORD かつ QUOTE_SINGLE でない
    ↓
    expand_variables() [expander_variable.c:86-128]
      ↓
      文字ごとにスキャン:
      ├─ update_quote_state() ... クォート状態更新
      ├─ '$' を検出 → process_variable()
      │   ├─ get_var_name_len() ... 変数名の長さを取得
      │   └─ get_env_value() ... 環境変数の値を取得
      │       ├─ "$?" → shell->last_status を文字列化
      │       └─ "$VAR" → getenv() で値取得
      └─ append_str() ... 結果文字列に追加
  ↓
output: 展開後のトークンリスト
[TOKEN_WORD("echo")] → [TOKEN_WORD("miyachirio")] → [TOKEN_WORD("0")]
```

#### 使用関数の詳細

| 関数名 | 場所 | Input | Output | 処理内容 |
|--------|------|-------|--------|----------|
| `expand_tokens()` | [expander/expander.c:15-39](expander/expander.c#L15-L39) | `t_token *tokens, t_shell *shell` | void | 全トークンを走査して変数展開を適用 |
| `expand_variables()` | [expander_variable.c:86-128](expander/expander_variable.c#L86-L128) | `char *str, t_shell *shell` | `char *` | 文字列内の$VAR, $?を展開 |
| `process_variable()` | [expander_variable.c:55-84](expander/expander_variable.c#L55-L84) | `char *str, int *i, t_shell *shell, t_quote_state *state` | `char *` | '$'以降を変数として処理 |
| `get_var_name_len()` | [expander_variable.c:15-27](expander/expander_variable.c#L15-L27) | `char *str` | `int` | 変数名の長さを計算（'?'は1, 英数字+_） |
| `get_env_value()` | [expander_variable.c:29-53](expander/expander_variable.c#L29-L53) | `char *var_name, int var_len, t_shell *shell` | `char *` | 環境変数の値を取得。$?は終了ステータス |
| `update_quote_state()` | expander_utils.c | `char c, t_quote_state *state` | void | 現在の文字がクォートか判定して状態更新 |
| `append_str()` | expander_utils.c | `char *result, char *str, int *i, int *res_len` | `char *` | 文字列を結果に追加（realloc） |

**重要ポイント**:

- 単一クォート `'...'` 内は展開しない
- 二重クォート `"..."` 内は展開する
- `$?` は最後のコマンドの終了ステータス

---

### 【機能3】PARSING (構文解析)

#### 処理フロー

```
tokens: [TOKEN_WORD("ls")] → [TOKEN_WORD("-la")] → [TOKEN_PIPE]
        → [TOKEN_WORD("grep")] → [TOKEN_WORD("test")]
        → [TOKEN_REDIR_OUT] → [TOKEN_WORD("out.txt")]
  ↓
parse() [parser/parser.c:91-120]
  ↓
┌─ パイプでコマンドを分割
│  ├─ parse_simple_command() ... パイプ間の1コマンドを解析
│  │   [parser/parser.c:56-89]
│  │   ├─ TOKEN_WORD → args配列に追加
│  │   └─ TOKEN_REDIR_* → handle_redirection()
│  │       [parser/parser.c:15-53]
│  │       ├─ リダイレクト種別を判定
│  │       ├─ ファイル名トークンを取得
│  │       ├─ HEREDOCの場合、expand_heredocフラグ設定
│  │       ├─ create_redirect() ... リダイレクト構造体作成
│  │       └─ add_redir_back() ... リストに追加
│  └─ add_cmd_back() ... コマンドをパイプラインに追加
└─ TOKEN_PIPE を消費して次へ
  ↓
output: コマンドパイプライン
cmd1 { args: ["ls", "-la", NULL], redir: NULL, next: cmd2 }
  ↓
cmd2 { args: ["grep", "test", NULL],
       redir: [REDIR_OUT("out.txt")],
       next: NULL }
```

#### 使用関数の詳細

| 関数名 | 場所 | Input | Output | 処理内容 |
|--------|------|-------|--------|----------|
| `parse()` | [parser/parser.c:91-120](parser/parser.c#L91-L120) | `t_token *tokens` | `t_cmd *` | トークン列をコマンドパイプラインに変換 |
| `parse_simple_command()` | [parser/parser.c:56-89](parser/parser.c#L56-L89) | `t_token **tokens` | `t_cmd *` | パイプ間の1つのコマンドをパース |
| `handle_redirection()` | [parser/parser.c:15-53](parser/parser.c#L15-L53) | `t_cmd *cmd, t_token **current` | `int` | リダイレクション処理（成功:1, 失敗:0） |
| `create_cmd()` | parser_utils.c | void | `t_cmd *` | 空のコマンド構造体を作成 |
| `create_redirect()` | parser_utils.c | `t_redir_type type, char *filename` | `t_redirect *` | リダイレクト構造体を作成 |
| `add_cmd_back()` | parser_utils.c | `t_cmd **list, t_cmd *new_cmd` | void | パイプラインの末尾にコマンド追加 |
| `add_redir_back()` | parser_utils.c | `t_redirect **list, t_redirect *new` | void | リダイレクトリストに追加 |
| `convert_list_to_array()` | parser_utils.c | `t_list *list` | `char **` | リンクリストをNULL終端配列に変換 |

**重要ポイント**:

- `<<` の後のトークンがクォートなし → `expand_heredoc = 1`
- `<<` の後のトークンがクォート付き → `expand_heredoc = 0`

---

### 【機能4】HEREDOC PROCESSING (ヒアドック処理)

#### 処理フロー

```
入力例:
cat << EOF
hello $USER
world
EOF

  ↓
process_heredocs() [heredoc.c:88-109]
  ↓
各コマンドのリダイレクトリストを走査
  ├─ REDIR_HEREDOC を検出
  │   ↓
  │   process_single_heredoc() [heredoc.c:75-86]
  │     ↓
  │     read_heredoc_content() [heredoc.c:25-73]
  │       ├─ mkstemp("/tmp/.heredoc_XXXXXX") ... 一時ファイル作成
  │       ├─ unlink(template) ... ファイル名を削除（fdは保持）
  │       ├─ setup_signals_heredoc() ... Ctrl+C対応
  │       ├─ while(1):
  │       │   ├─ readline("> ") ... 1行読み込み
  │       │   ├─ delimiter と比較 → 一致したら終了
  │       │   ├─ expand_heredoc=1 なら expand_variables()
  │       │   └─ write(fd, line, ...) ... 一時ファイルに書き込み
  │       └─ lseek(fd, 0, SEEK_SET) ... 先頭に戻す
  │     ↓
  │     redir->heredoc_fd = fd ... fdを保存
  └─ 次のリダイレクトへ
  ↓
output: heredoc_fd にファイルディスクリプタが格納された状態
```

#### 使用関数の詳細

| 関数名 | 場所 | Input | Output | 処理内容 |
|--------|------|-------|--------|----------|
| `process_heredocs()` | [heredoc.c:88-109](heredoc.c#L88-L109) | `t_cmd *pipeline, t_shell *shell` | `int` | 全ヒアドックを処理（成功:1, 失敗:0） |
| `process_single_heredoc()` | [heredoc.c:75-86](heredoc.c#L75-L86) | `t_redirect *redir, t_shell *shell` | `int` | 1つのヒアドックを処理 |
| `read_heredoc_content()` | [heredoc.c:25-73](heredoc.c#L25-L73) | `char *delimiter, int expand, t_shell *shell` | `int` (fd) | 内容を読み込み一時ファイルに保存 |
| `setup_signals_heredoc()` | [heredoc.c:15-23](heredoc.c#L15-L23) | void | void | ヒアドック入力中のシグナル設定 |
| `cleanup_heredocs()` | [heredoc.c:111-131](heredoc.c#L111-L131) | `t_cmd *pipeline` | void | 全ヒアドックのfdをclose |

**重要ポイント**:

- 一時ファイルは `mkstemp()` で作成後すぐに `unlink()` される
- ファイル名は削除されるが、fdは保持され続ける
- Ctrl+C で中断可能

---

### 【機能5】EXECUTION (コマンド実行)

#### 処理フロー

##### 単一コマンドの場合

```
pipeline: cmd { args: ["ls", "-la", NULL], redir: NULL, next: NULL }
  ↓
execute_pipeline() [pipe.c:120-187]
  ├─ count_commands() → 1
  ├─ リダイレクションなし → execute_with_args()
  │   [execute.c:3-43]
  │   ├─ get_command_path() ... PATHから実行ファイルを検索
  │   ├─ fork() ... 子プロセス生成
  │   ├─ 子プロセス:
  │   │   ├─ setup_signals_child() ... シグナルをデフォルトに戻す
  │   │   └─ execve(cmd_path, args, environ) ... 実行
  │   └─ 親プロセス:
  │       └─ waitpid() ... 子プロセスの終了を待つ
  └─ return 終了ステータス
```

##### パイプライン実行の場合

```
pipeline: cmd1 → cmd2 → cmd3
  ↓
execute_pipeline() [pipe.c:120-187]
  ├─ count_commands() → 3
  ├─ prev_pipe_fd = {-1, -1}
  │
  ├─ [コマンド1] ───────────────────────────────
  │   ├─ pipe(pipe_fd) ... 新しいパイプ作成 [R, W]
  │   ├─ fork() → 子プロセス1
  │   │   ├─ 前のパイプなし（最初のコマンド）
  │   │   ├─ dup2(pipe_fd[1], STDOUT) ... stdoutをパイプに接続
  │   │   ├─ close(pipe_fd[0], pipe_fd[1])
  │   │   ├─ apply_redirections() ... リダイレクション適用
  │   │   └─ execve(cmd1)
  │   ├─ 親: prev_pipe_fd = pipe_fd を保存
  │
  ├─ [コマンド2] ───────────────────────────────
  │   ├─ pipe(pipe_fd) ... 新しいパイプ作成
  │   ├─ fork() → 子プロセス2
  │   │   ├─ dup2(prev_pipe_fd[0], STDIN) ... 前のパイプから入力
  │   │   ├─ dup2(pipe_fd[1], STDOUT) ... 次のパイプへ出力
  │   │   ├─ close(全パイプfd)
  │   │   ├─ apply_redirections()
  │   │   └─ execve(cmd2)
  │   ├─ 親: close(prev_pipe_fd)
  │   └─ 親: prev_pipe_fd = pipe_fd を保存
  │
  ├─ [コマンド3] ───────────────────────────────
  │   ├─ パイプ作成なし（最後のコマンド）
  │   ├─ fork() → 子プロセス3
  │   │   ├─ dup2(prev_pipe_fd[0], STDIN) ... 前のパイプから入力
  │   │   ├─ stdoutはそのまま（最後のコマンド）
  │   │   ├─ apply_redirections()
  │   │   └─ execve(cmd3)
  │   └─ 親: close(prev_pipe_fd)
  │
  └─ 全子プロセスを wait() ... 3回
     └─ return 最後のコマンドの終了ステータス
```

#### apply_redirections() の詳細 [pipe.c:15-68](pipe.c#L15-L68)

```
redir_list を走査:
  ├─ REDIR_IN (<)
  │   ├─ fd = open(filename, O_RDONLY)
  │   ├─ dup2(fd, STDIN_FILENO)
  │   └─ close(fd)
  │
  ├─ REDIR_OUT (>)
  │   ├─ fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644)
  │   ├─ dup2(fd, STDOUT_FILENO)
  │   └─ close(fd)
  │
  ├─ REDIR_APPEND (>>)
  │   ├─ fd = open(filename, O_WRONLY|O_CREAT|O_APPEND, 0644)
  │   ├─ dup2(fd, STDOUT_FILENO)
  │   └─ close(fd)
  │
  └─ REDIR_HEREDOC (<<)
      ├─ heredoc_fd が保存されている
      └─ dup2(heredoc_fd, STDIN_FILENO)
```

#### 使用関数の詳細

| 関数名 | 場所 | Input | Output | 処理内容 |
|--------|------|-------|--------|----------|
| `execute_pipeline()` | [pipe.c:120-187](pipe.c#L120-L187) | `t_cmd *pipeline, t_shell *shell` | `int` | パイプライン全体を実行 |
| `execute_with_args()` | [execute.c:3-43](execute.c#L3-L43) | `char **args` | `int` | 単一コマンドを実行 |
| `execute_child()` | [pipe.c:70-103](pipe.c#L70-L103) | `t_cmd *cmd, int *prev_pipe, int *pipe_fd, int is_last` | void | 子プロセス内の実行処理 |
| `apply_redirections()` | [pipe.c:15-68](pipe.c#L15-L68) | `t_redirect *redir_list` | `int` | 全リダイレクションを適用 |
| `count_commands()` | [pipe.c:105-118](pipe.c#L105-L118) | `t_cmd *pipeline` | `int` | パイプライン内のコマンド数をカウント |
| `get_command_path()` | path.c | `char *cmd` | `char *` | PATHから実行ファイルの絶対パスを検索 |

---

## 📊 全関数のI/O一覧

### main.c

| 関数 | 行 | Input | Output | 役割 |
|------|-----|-------|--------|------|
| `main()` | 46-68 | `int argc, char **argv, char **envp` | `int` | エントリーポイント。REPLループ |
| `process_input()` | 5-43 | `char *input, t_shell *shell` | void | 入力を5段階処理 |

### tokenizer/tokenizer.c

| 関数 | 行 | Input | Output | 役割 |
|------|-----|-------|--------|------|
| `tokenize()` | 3-46 | `char *input` | `t_token *` | 入力文字列をトークン化 |

### parser/parser.c

| 関数 | 行 | Input | Output | 役割 |
|------|-----|-------|--------|------|
| `parse()` | 91-120 | `t_token *tokens` | `t_cmd *` | トークンをコマンドパイプラインに変換 |
| `parse_simple_command()` | 56-89 | `t_token **tokens` | `t_cmd *` | パイプ間の1コマンドをパース |
| `handle_redirection()` | 15-53 | `t_cmd *cmd, t_token **current` | `int` | リダイレクション処理 |

### expander/expander.c

| 関数 | 行 | Input | Output | 役割 |
|------|-----|-------|--------|------|
| `expand_tokens()` | 15-39 | `t_token *tokens, t_shell *shell` | void | 全トークンの変数展開 |

### expander/expander_variable.c

| 関数 | 行 | Input | Output | 役割 |
|------|-----|-------|--------|------|
| `expand_variables()` | 86-128 | `char *str, t_shell *shell` | `char *` | 文字列内の変数を展開 |
| `process_variable()` | 55-84 | `char *str, int *i, t_shell *shell, t_quote_state *state` | `char *` | $以降を変数として処理 |
| `get_var_name_len()` | 15-27 | `char *str` | `int` | 変数名の長さを計算 |
| `get_env_value()` | 29-53 | `char *var_name, int var_len, t_shell *shell` | `char *` | 環境変数の値取得 |

### heredoc.c

| 関数 | 行 | Input | Output | 役割 |
|------|-----|-------|--------|------|
| `process_heredocs()` | 88-109 | `t_cmd *pipeline, t_shell *shell` | `int` | 全ヒアドックを処理 |
| `process_single_heredoc()` | 75-86 | `t_redirect *redir, t_shell *shell` | `int` | 1つのヒアドック処理 |
| `read_heredoc_content()` | 25-73 | `char *delimiter, int expand, t_shell *shell` | `int` (fd) | 内容を一時ファイルに保存 |
| `setup_signals_heredoc()` | 15-23 | void | void | ヒアドック中のシグナル設定 |
| `cleanup_heredocs()` | 111-131 | `t_cmd *pipeline` | void | 全ヒアドックfdをclose |

### pipe.c

| 関数 | 行 | Input | Output | 役割 |
|------|-----|-------|--------|------|
| `execute_pipeline()` | 120-187 | `t_cmd *pipeline, t_shell *shell` | `int` | パイプライン全体を実行 |
| `execute_child()` | 70-103 | `t_cmd *cmd, int *prev_pipe, int *pipe_fd, int is_last` | void | 子プロセスの実行処理 |
| `apply_redirections()` | 15-68 | `t_redirect *redir_list` | `int` | 全リダイレクション適用 |
| `count_commands()` | 105-118 | `t_cmd *pipeline` | `int` | コマンド数カウント |

### execute.c

| 関数 | 行 | Input | Output | 役割 |
|------|-----|-------|--------|------|
| `execute_with_args()` | 3-43 | `char **args` | `int` | 単一コマンド実行 |

---

## 🔄 データフロー図

### データ構造の変遷

```
[ユーザー入力]
  "ls -la | grep test > out.txt"
      ↓
[文字列 (char *)]
      ↓ tokenize()
[トークンリスト (t_token *)]
  TOKEN_WORD("ls") → TOKEN_WORD("-la") → TOKEN_PIPE
  → TOKEN_WORD("grep") → TOKEN_WORD("test")
  → TOKEN_REDIR_OUT(">") → TOKEN_WORD("out.txt")
      ↓ expand_tokens()
[展開済みトークンリスト]
  (変数が環境変数値に置換される)
      ↓ parse()
[コマンドパイプライン (t_cmd *)]
  cmd1 {
    args: ["ls", "-la", NULL],
    redir: NULL,
    next: cmd2
  }
  cmd2 {
    args: ["grep", "test", NULL],
    redir: [REDIR_OUT("out.txt")],
    next: NULL
  }
      ↓ process_heredocs()
[ヒアドック処理済みパイプライン]
  (heredoc_fd にファイルディスクリプタが設定される)
      ↓ execute_pipeline()
[プロセス実行]
  fork() → pipe() → dup2() → execve()
      ↓
[終了ステータス (int)]
  shell->last_status に保存
```

### 各機能間のつながり

```
┌──────────────┐
│  main()      │
│  [main.c]    │
└──────┬───────┘
       │
       ↓ readline()
┌──────────────────────────────────────────────────────┐
│  process_input()                    [main.c:5-43]    │
├──────────────────────────────────────────────────────┤
│                                                      │
│  [1] tokenize()              [tokenizer/tokenizer.c] │
│      ├─ skip_space()                                 │
│      ├─ is_special()                                 │
│      ├─ tokenize_special() / get_word()              │
│      ├─ get_quote_type()                             │
│      ├─ strip_quotes()                               │
│      ├─ create_token()                               │
│      └─ add_token()                                  │
│         ↓                                            │
│  [2] expand_tokens()            [expander/expander.c]│
│      └─ expand_variables()   [expander_variable.c]   │
│          ├─ update_quote_state()                     │
│          ├─ process_variable()                       │
│          │   ├─ get_var_name_len()                   │
│          │   └─ get_env_value()                      │
│          └─ append_str()                             │
│         ↓                                            │
│  [3] parse()                      [parser/parser.c]  │
│      ├─ parse_simple_command()                       │
│      │   └─ handle_redirection()                     │
│      │       └─ create_redirect()                    │
│      └─ add_cmd_back()                               │
│         ↓                                            │
│  [4] process_heredocs()              [heredoc.c]     │
│      └─ process_single_heredoc()                     │
│          └─ read_heredoc_content()                   │
│              ├─ setup_signals_heredoc()              │
│              └─ expand_variables() (再利用)          │
│         ↓                                            │
│  [5] execute_pipeline()               [pipe.c]      │
│      ├─ count_commands()                             │
│      ├─ execute_with_args()        [execute.c]      │
│      │   └─ get_command_path()     [path.c]         │
│      └─ execute_child()                              │
│          └─ apply_redirections()                     │
│         ↓                                            │
│  [6] cleanup & free                                  │
│      ├─ cleanup_heredocs()                           │
│      └─ free_pipeline()                              │
│                                                      │
└──────────────────────────────────────────────────────┘
```

---

## 🎯 重要な処理のポイント

### 1. メモリ管理

- トークンリストは [main.c:25](main.c#L25) でparse後に解放
- パイプラインは [main.c:42](main.c#L42) で実行後に解放
- ヒアドックfdは [main.c:41](main.c#L41) でcleanup

### 2. エラーハンドリング

- 構文エラー: `handle_syntax_error()` で処理
- ファイルオープンエラー: `perror()` で出力して終了
- コマンド未発見: 終了ステータス127

### 3. シグナル処理

- 対話モード: Ctrl+C で現在の行をキャンセル
- 子プロセス: デフォルトの動作に戻す
- ヒアドック入力中: Ctrl+C で中断可能

### 4. パイプ処理の重要な点

- 各コマンドの前に新しいパイプを作成
- 子プロセスはパイプの読み込み端を stdin に、書き込み端を stdout に接続
- 親プロセスは使用済みパイプを必ず close

---

この解説書により、minishellの全体像から各関数の詳細まで、完全に把握できるはずです。各機能がどのような順序で処理され、どの関数が使われているかが明確になっています。

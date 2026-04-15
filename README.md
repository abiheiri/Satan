# Satan

Testing out how tcp listener daemon works in C while also testing out how to build packages in Github for C applications.

## Usage

### build and run

```bash
 gcc -Wall -o satan satan.c;
  
./satan
```

### test

```bash
 (echo "echo hello"; sleep 3) | nc localhost 6666
```

# UUID v7 - Go

Binding Go untuk library UUID v7.

## Penggunaan

```go
package main

import (
    "fmt"
    "github.com/username/uuid7"
)

func main() {
    gen, _ := uuid7.NewGenerator()
    defer gen.Close()

    u := gen.Generate()
    fmt.Printf("%x\n", u)
}
```

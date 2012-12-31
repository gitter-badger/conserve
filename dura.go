package main

import (
	"flag"
	"fmt"
	"github.com/sourcefrog/dura/duralib"
	"os"
)

func main() {
	flag.Parse()
	if flag.NArg() != 1 {
		fmt.Printf("usage: dura ARCHIVE_DIR\n")
		os.Exit(1)
	}

	err := duralib.DescribeArchive(flag.Arg(0))
	if err != nil {
		fmt.Printf("%s\n", err.Error())
		os.Exit(2)
	}
}

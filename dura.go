package main

import (
	"code.google.com/p/goprotobuf/proto"
	"errors"
	"flag"
	"fmt"
	"github.com/sourcefrog/dura/proto"
	"io"
	"os"
)

func main() {
	flag.Parse()
	if flag.NArg() != 1 {
		fmt.Printf("usage: dura ARCHIVE_DIR\n")
		os.Exit(1)
	}

	err := DescribeArchive(flag.Arg(0))
	if err != nil {
		fmt.Printf("%s\n", err.Error())
		os.Exit(2)
	}
}

type Archive struct {
}

func readArchiveHeader(archive_dir string) (
	header duralib_proto.ArchiveHeader,
	err error) {
	header_file, err := os.Open(archive_dir + "/DURA-ARCHIVE")
	if err != nil {
		return
	}

	defer header_file.Close()

	header_stat, err := header_file.Stat()
	if err != nil {
		return
	}

	buffer := make([]byte, header_stat.Size())

	_, err = io.ReadFull(header_file, buffer)
	if err != nil {
		return
	}

	err = proto.Unmarshal(buffer, &header)
	if err != nil {
		return
	}

	return header, nil
}

func OpenArchive(archive_dir string) (archive Archive, err error) {
	dir_stat, err := os.Stat(archive_dir)
	if err != nil {
		return
	}
	if !dir_stat.IsDir() {
		err = errors.New(
			fmt.Sprintf("%s is not a directory", archive_dir))
		return
	}

	header, err := readArchiveHeader(archive_dir)
	fmt.Printf("header magic: %q\n", *header.Magic)
	fmt.Printf("flags: read=%v, write=%v\n", header.RequiredReadFlags,
		header.RequiredWriteFlags)

	return Archive{}, nil
}

func DescribeArchive(archive_dir string) error {

	_, err := OpenArchive(archive_dir)
	if err != nil {
		return err
	}
	return nil
}

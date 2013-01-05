package duralib_test

import (
	"io/ioutil"
	"github.com/sourcefrog/dura/duralib"
	"testing"
)

func TestOpenNonExistent(t *testing.T) {
	archive, err := duralib.OpenArchive("/nonexistent")
	if archive != nil {
		t.Error("got an archive unexpectedly: %v", archive)
	}
	if err == nil {
		t.Error("no error opening a nonexistent archive")
	}
}

func testDirectory() (string, error) {
	return ioutil.TempDir("", "duratest")
}

func TestCreateArchive(t *testing.T) {
	testDir, err := testDirectory()
	if err != nil {
		t.Error(err.Error())
	}
	archive, err := duralib.CreateArchive(testDir)
	if err != nil {
		t.Error(err.Error())
	}
	if archive == nil {
		t.Error("nil archive returned")
	}
}

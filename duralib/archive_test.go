package duralib_test

import (
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

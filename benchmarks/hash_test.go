package main

import (
	"crypto/md5"
	"crypto/rand"
	"crypto/sha1"
	"crypto/sha256"
	"crypto/sha512"
	"fmt"
	"hash"
	"testing"

	"golang.org/x/crypto/blake2b"
	"golang.org/x/crypto/blake2s"
	"golang.org/x/crypto/sha3"
)

var result []byte
var benchmarks map[string][]byte

func create(size int) []byte {
	result := make([]byte, size)
	rand.Read(result)
	return result
}

func init() {
	benchmarks = make(map[string][]byte)
	for i := 10; i <= 1000000; i *= 10 {
		name := fmt.Sprint(i)
		benchmarks[name] = create(i)
	}
}

func benchmarkHash(b *testing.B, h hash.Hash) {
	for name, data := range benchmarks {
		b.Run(name, func(b *testing.B) {
			var r []byte
			for i := 0; i < b.N; i++ {
				h.Write(data)
				r = h.Sum(nil)
			}
			result = r
		})
	}
}

func BenchmarkMD5(b *testing.B) {
	benchmarkHash(b, md5.New())
}

func BenchmarkSHA1(b *testing.B) {
	benchmarkHash(b, sha1.New())
}

func BenchmarkSHA224(b *testing.B) {
	benchmarkHash(b, sha256.New224())
}

func BenchmarkSHA256(b *testing.B) {
	benchmarkHash(b, sha256.New())
}

func BenchmarkSHA384(b *testing.B) {
	benchmarkHash(b, sha512.New384())
}

func BenchmarkSHA512(b *testing.B) {
	benchmarkHash(b, sha512.New())
}

func BenchmarkSHA3_224(b *testing.B) {
	benchmarkHash(b, sha3.New224())
}

func BenchmarkSHA3_256(b *testing.B) {
	benchmarkHash(b, sha3.New256())
}

func BenchmarkSHA3_384(b *testing.B) {
	benchmarkHash(b, sha3.New384())
}

func BenchmarkSHA3_512(b *testing.B) {
	benchmarkHash(b, sha3.New512())
}

func BenchmarkSHA512_224(b *testing.B) {
	benchmarkHash(b, sha512.New512_224())
}

func BenchmarkSHA512_256(b *testing.B) {
	benchmarkHash(b, sha512.New512_256())
}

func BenchmarkBLAKE2s_256(b *testing.B) {
	h, _ := blake2s.New256(nil)
	benchmarkHash(b, h)
}

func BenchmarkBLAKE2b_256(b *testing.B) {
	h, _ := blake2b.New256(nil)
	benchmarkHash(b, h)
}

func BenchmarkBLAKE2b_384(b *testing.B) {
	h, _ := blake2b.New384(nil)
	benchmarkHash(b, h)
}

func BenchmarkBLAKE2b_512(b *testing.B) {
	h, _ := blake2b.New512(nil)
	benchmarkHash(b, h)
}

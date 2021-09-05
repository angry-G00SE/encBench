package main

import (
	"crypto/ed25519"
	"crypto/rand"
	"testing"
)

func BenchmarkED25519_Sign(b *testing.B) {
	for name, data := range benchmarks {
		_, pr, _ := ed25519.GenerateKey(rand.Reader)

		b.Run(name, func(b *testing.B) {
			var r []byte
			for i := 0; i < b.N; i++ {
				r = ed25519.Sign(pr, data)
			}
			result = r
		})
	}
}

var verify bool

func BenchmarkED25519_Verify(b *testing.B) {
	signatures := make(map[string][]byte)
	pb, pr, _ := ed25519.GenerateKey(rand.Reader)

	for name, data := range benchmarks {
		signatures[name] = ed25519.Sign(pr, data)
	}

	for name := range benchmarks {
		b.Run(name, func(b *testing.B) {
			var r bool
			for i := 0; i < b.N; i++ {
				r = ed25519.Verify(pb, signatures[name], result)
			}
			verify = r
		})
	}
}

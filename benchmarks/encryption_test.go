package main

import (
	"crypto/aes"
	"crypto/cipher"
	"crypto/rand"
	"crypto/rsa"
	"crypto/sha256"
	"testing"
)

func encryptAES(data []byte, key []byte) []byte {
	c, _ := aes.NewCipher(key)

	gcm, _ := cipher.NewGCM(c)

	nonce := make([]byte, gcm.NonceSize())

	rand.Read(nonce)

	return gcm.Seal(nonce, nonce, data, nil)
}

func benchmark_AES_Encrypt(b *testing.B, blockSize int) {
	key := make([]byte, blockSize)
	c, _ := aes.NewCipher(key)

	gcm, _ := cipher.NewGCM(c)

	nonce := make([]byte, gcm.NonceSize())

	rand.Read(nonce)

	b.ResetTimer()

	for name, data := range benchmarks {
		b.Run(name, func(b *testing.B) {
			var r []byte
			for i := 0; i < b.N; i++ {
				r = gcm.Seal(nonce, nonce, data, nil)
			}
			result = r
		})
	}
}

func benchmark_AES_Decrypt(b *testing.B, blockSize int) {
	encryptions := make(map[string][]byte)

	key := make([]byte, blockSize)
	rand.Read(key)

	for name, data := range benchmarks {
		encryptions[name] = encryptAES(data, key)
	}

	c, _ := aes.NewCipher(key)

	gcm, _ := cipher.NewGCM(c)

	nonceSize := gcm.NonceSize()

	b.ResetTimer()

	for name := range benchmarks {
		b.Run(name, func(b *testing.B) {
			var r []byte
			ciphertext := encryptions[name]

			for i := 0; i < b.N; i++ {
				nonce, ciphertext := ciphertext[:nonceSize], ciphertext[nonceSize:]
				r, _ = gcm.Open(nil, nonce, ciphertext, nil)
			}
			result = r
		})
	}
}

func BenchmarkAES_Encrypt_128(b *testing.B) {
	benchmark_AES_Encrypt(b, 16)
}

func BenchmarkAES_Encrypt_192(b *testing.B) {
	benchmark_AES_Encrypt(b, 24)
}

func BenchmarkAES_Encrypt_256(b *testing.B) {
	benchmark_AES_Encrypt(b, 32)
}

func BenchmarkAES_Decrypt_128(b *testing.B) {
	benchmark_AES_Decrypt(b, 16)
}

func BenchmarkAES_Decrypt_192(b *testing.B) {
	benchmark_AES_Decrypt(b, 24)
}

func BenchmarkAES_Decrypt_256(b *testing.B) {
	benchmark_AES_Decrypt(b, 32)
}

func BenchmarkRSA_Encrypt(b *testing.B) {
	pr, _ := rsa.GenerateKey(rand.Reader, 2048)

	b.ResetTimer()

	for name, data := range benchmarks {
		b.Run(name, func(b *testing.B) {
			var r []byte
			for i := 0; i < b.N; i++ {
				r, _ = rsa.EncryptOAEP(sha256.New(), rand.Reader, &pr.PublicKey, data, nil)
			}
			result = r
		})
	}
}

func BenchmarkRSA_Decrypt(b *testing.B) {
	pr, _ := rsa.GenerateKey(rand.Reader, 2048)

	encryptions := make(map[string][]byte)

	for name, data := range benchmarks {
		encryptions[name], _ = rsa.EncryptOAEP(sha256.New(), rand.Reader, &pr.PublicKey, data, nil)
	}

	b.ResetTimer()

	for name := range benchmarks {
		b.Run(name, func(b *testing.B) {
			var r []byte
			for i := 0; i < b.N; i++ {
				r, _ = rsa.DecryptOAEP(sha256.New(), rand.Reader, pr, encryptions[name], nil)
			}
			result = r
		})
	}
}

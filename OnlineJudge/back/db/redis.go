package db

import (
	"context"
	"github.com/redis/go-redis/v9"
	"time"
)

var rdb *redis.Client

func InitRedis() {
	rdb = redis.NewClient(&redis.Options{
		Addr:     "localhost:6379",
		Password: "", // no password set
		DB:       0,  // use default DB
	})
	// 测试连接
	_, err := rdb.Ping(context.Background()).Result()
	if err != nil {
		panic("Redis connection failed: " + err.Error())
	}
}
func SetVerifyCode(key string, value string) error {
	ctx := context.Background()
	err := rdb.Set(ctx, key, value, 2*time.Hour).Err()
	return err
}
func GetVerifyCode(key string) (string, error) {
	ctx := context.Background()
	value, err := rdb.Get(ctx, key).Result()
	if err != nil {
		return "", err
	}
	return value, nil
}

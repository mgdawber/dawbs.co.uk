package main

import (
	"github.com/gin-gonic/gin"
	"github.com/mgdawber/dawbs-api/pkg/posts"
	"github.com/mgdawber/dawbs-api/pkg/common/db"
	"github.com/spf13/viper"
)

func main() {
	viper.SetConfigFile("./pkg/common/envs/.env")
	viper.ReadInConfig()

	port := viper.Get("PORT").(string)
	dbUrl := viper.Get("DB_URL").(string)

	r := gin.Default()
    h := db.Init(dbUrl)

    posts.RegisterRoutes(r, h)

    r.Run(port)
}

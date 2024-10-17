package main

import (
	"fmt"

	"gorm.io/driver/sqlite"
	"gorm.io/gorm"
)

type Product struct {
	gorm.Model
	Code  string
	Price uint
}

func main() {
	db, err := gorm.Open(sqlite.Open("test.db"), &gorm.Config{
		FullSaveAssociations: true,
		DryRun:               true,
	})
	if err != nil {
		panic("failed to connect database")
	}

	// statement := db.Create(&Product{Code: "D42", Price: 100}).Statement
	// args := statement.Vars
	// fmt.Println(statement.SQL.String())
	// fmt.Println(args)

	// statement = db.Model(&Product{}).Create(map[string]any{
	// 	"Code":  "D42",
	// 	"Price": 100,
	// }).Statement
	// args = statement.Vars
	// fmt.Println(statement.SQL.String())
	// fmt.Println(args)

	// statement = db.Model(&Product{
	// 	Code: "D42",
	// }).Create(map[string]any{
	// 	"Price": 100,
	// }).Statement
	// args = statement.Vars
	// fmt.Println(statement.SQL.String())
	// fmt.Println(args)
	Main(db)
}

// 文章
type Topics struct {
	Id       int    `gorm:"primary_key"`
	Title    string `gorm:"not null"`
	Category Categories
	User     Users
}

// 用户
type Users struct {
	Id   int    `gorm:"primary_key"`
	Name string `gorm:"not null"`
}

// 分类
type Categories struct {
	Id   int    `gorm:"primary_key"`
	Name string `gorm:"not null"`
}

func Main(db *gorm.DB) {
	models := []interface{}{
		&Topics{},
		&Users{},
		&Categories{},
	}
	//1.执行建表语句
	err := db.Debug().AutoMigrate(models...)
	if err != nil {
		fmt.Println("db error:", err)
	}

	statement := db.Create(&Topics{
		Title: "测试",
		Category: Categories{
			Name: "测试分类",
		},
		User: Users{
			Name: "测试用户",
		},
	}).Statement
	fmt.Println(statement.SQL.String())
	fmt.Println(statement.Vars)
	//2.执行sql
	//INSERT INTO topics("id", "title", "user_id", "category_id") VALUES (1, '测试', 1, 1);
	//INSERT INTO categories("id", "name") VALUES (1, '测试分类');
	//INSERT INTO users("id", "name") VALUES (1, '测试用户');

	// //3.执行预加载
	// topics, err := GetTopicsById(db, 1)
	// if err != nil {
	// 	fmt.Println("get topics error:", err)
	// }
	// fmt.Println(topics)
}

func GetTopicsById(db *gorm.DB, id int) (*Topics, error) {
	var topic Topics
	//查询方法1
	//err := db.Model(&topic).Where("id=?", id).First(&topic).
	//	Related(&topic.Category, "CategoryId").
	//	Related(&topic.User, "UserId").Error

	//查询方法2
	err := db.Where("id=?", id).
		Preload("Category").
		Preload("User").
		First(&topic).Error
	if err != nil {
		return nil, err
	}
	return &topic, nil
}

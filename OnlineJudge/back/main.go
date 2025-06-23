package main

import (
	"back/db"
	"back/model"
	"crypto/tls"
	"errors"
	"fmt"
	"github.com/gin-gonic/gin"
	"github.com/golang-jwt/jwt/v5"
	"github.com/shirou/gopsutil/v3/cpu"
	"github.com/shirou/gopsutil/v3/disk"
	"github.com/shirou/gopsutil/v3/host"
	"github.com/shirou/gopsutil/v3/load"
	"github.com/shirou/gopsutil/v3/mem"
	"github.com/shirou/gopsutil/v3/net"
	"gopkg.in/gomail.v2"
	"gorm.io/gorm"
	"math/rand"
	"net/http"
	"strconv"
	"strings"
	"time"
)

func main() {
	db.InitDB()
	db.InitRedis()
	//gin.SetMode(gin.ReleaseMode)
	r := gin.Default()
	//登录注册
	r.POST("/login", login)
	r.POST("/register", register)
	r.POST("/verify", sendMail)
	//主页
	r.GET("/", home)
	r.GET("/admin", dashboard)
	//群组
	r.GET("/group", getGroupList)                                     //获取群组列表
	r.GET("/group/:groupName", AuthMiddleware(), getGroupById)        //查看群组详情
	r.POST("/group/:groupName/apply", AuthMiddleware(), applyGroup)   //自己申请
	r.POST("/group/:groupName/invite", AuthMiddleware(), inviteGroup) //邀请别人
	r.GET("/message", AuthMiddleware(), getInfoGroup)                 //获取申请信息
	r.POST("/group/operate", AuthMiddleware(), operateGroup)          //批准/拒绝申请
	r.POST("/group", AuthMiddleware(), createGroup)                   //创建群组

	//新闻
	r.GET("/new", getNewList)
	r.GET("/new/:ID", getNewById)                          //查看新闻详情
	r.POST("/new", AuthMiddlewareAdmin(), createNew)       //创建新闻
	r.PUT("/new/:ID", AuthMiddlewareAdmin(), updateNew)    //删除新闻
	r.DELETE("/new/:ID", AuthMiddlewareAdmin(), deleteNew) //修改新闻

	//题目
	r.GET("/problem", getProblemList)                              //获取题目列表
	r.GET("/problem/:ID", getProblemByID)                          //查看题目详情
	r.POST("/problem", AuthMiddlewareAdmin(), createProblem)       //创建题目
	r.PUT("/problem/:ID", AuthMiddlewareAdmin(), updateProblem)    //删除题目
	r.DELETE("/problem/:ID", AuthMiddlewareAdmin(), deleteProblem) //修改题目

	//竞赛
	r.GET("/contest", AuthMiddleware(), getContestList)            //获取竞赛列表
	r.GET("/contest/:ID", AuthMiddleware(), getContestByID)        //查看竞赛
	r.GET("/contest/:ID/problems", getContestProblemList)          //查看竞赛的问题列表
	r.GET("/contest/:ID/problem/:PID", getContestProblemByID)      //查看竞赛的问题
	r.POST("/contest", AuthMiddlewareAdmin(), createContest)       //创建竞赛
	r.PUT("/contest/:ID", AuthMiddlewareAdmin(), updateContest)    //修改竞赛
	r.DELETE("/contest/:ID", AuthMiddlewareAdmin(), deleteContest) //删除竞赛
	err := r.Run("localhost:8080")
	if err != nil {
		fmt.Println("启动失败" + err.Error())
		return
	}

}

func dashboard(context *gin.Context) {
	//综合
	var data model.Log
	if v, err := mem.VirtualMemory(); err == nil {
		data.Mem = float64(int(v.UsedPercent*10)) / 10
	}
	if v, err := cpu.Percent(time.Second, false); err == nil {
		data.CPU = float64(int(v[0]*10)) / 10
	}
	if v, err := disk.Usage("/"); err == nil {
		data.Disk = float64(int(v.UsedPercent*10)) / 10
	}
	if v, err := net.IOCounters(false); err == nil {
		data.Recv = v[0].BytesRecv / 1024 / 1024
		data.Send = v[0].BytesSent / 1024 / 1024
	}
	if loadAvg, err := load.Avg(); err == nil {
		data.Load = float64(int(loadAvg.Load1*1000)) / 10
	}
	if info, err := host.Info(); err == nil {
		data.HostInfo = fmt.Sprintf("主机名:%s\n操作系统:%s\n内核版本:%s\n系统版本:%s\n系统架构:%s\n系统类型:%s\n系统时间:%s\n主机UUID:%s\n主机ID:%d",
			info.Hostname, info.OS, info.KernelVersion, info.PlatformVersion, info.KernelArch, info.Platform, info.BootTime, info.HostID, info.HostID)
	}
	if info, err := cpu.Info(); err == nil {
		data.CpuInfo = fmt.Sprintf("CPU型号:%s\nCPU核心数:%d\nCPU逻辑核心数:%d\nCPU频率:%f\nCPU缓存大小:%d\n",
			info[0].ModelName, info[0].Cores, info[0].Cores, info[0].Mhz, info[0].CacheSize)
	}
	context.JSON(http.StatusOK, data)
	return
}

func deleteContest(context *gin.Context) {
	id, err := strconv.Atoi(context.Param("ID"))
	if err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
		return
	}
	if err = db.DeleteContest(id); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "删除比赛失败\n" + err.Error()})
		return
	}
	context.AbortWithStatus(http.StatusOK)
}

func updateContest(context *gin.Context) {
	var contestData model.Contest
	id, err := strconv.Atoi(context.Param("ID"))
	if err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
		return
	}
	contestData.ID = id
	if err := context.ShouldBindJSON(&contestData); err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
		return
	}
	if err := db.UpdateContest(contestData); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "更新比赛失败\n" + err.Error()})
		return
	}
	context.AbortWithStatus(http.StatusCreated)
}

func createContest(context *gin.Context) {
	var contestData model.Contest
	if err := context.ShouldBindJSON(&contestData); err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
		return
	}
	if err := db.CreateContest(contestData); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "创建题比赛失败\n" + err.Error()})
		return
	}
	context.AbortWithStatus(http.StatusCreated)
}

func getContestByID(context *gin.Context) {
	id, err := strconv.Atoi(context.Param("ID"))
	if err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
		return
	}
	if context.MustGet("role").(bool) {
		contestInfo, err := db.GetContestById(id, true)
		if err != nil {
			context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "查询比赛失败\n" + err.Error()})
			return
		}
		context.AbortWithStatusJSON(http.StatusOK, contestInfo)
	} else {
		contestInfo, err := db.GetContestById(id, false)
		if err != nil {
			context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "查询比赛失败\n" + err.Error()})
			return
		}
		context.AbortWithStatusJSON(http.StatusOK, contestInfo)
	}
}

func getContestList(context *gin.Context) {
	var contestData model.GetContestDTO
	if err := context.ShouldBindQuery(&contestData); err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": err.Error()})
	}
	if context.MustGet("role").(bool) {
		contests, err := db.GetContestList(contestData, true)
		if err != nil {
			context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "查询比赛失败\n" + err.Error()})
			return
		}
		context.AbortWithStatusJSON(http.StatusOK, contests)
	} else {
		contests, err := db.GetContestList(contestData, false)
		if err != nil {
			context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "查询比赛失败\n" + err.Error()})
			return
		}
		context.AbortWithStatusJSON(http.StatusOK, contests)
	}
}

func getContestProblemList(context *gin.Context) {
	contestID, err := strconv.Atoi(context.Param("ID"))
	if err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
		return
	}
	if contestProblems, err := db.GetContestProblemList(contestID); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "查询比赛题目失败\n" + err.Error()})
	} else {
		context.AbortWithStatusJSON(http.StatusOK, contestProblems)
	}
}

func getContestProblemByID(context *gin.Context) {
	id, err := strconv.Atoi(context.Param("PID"))
	if err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
		return
	}
	if problemInfo, err := db.GetContestProblemById(id); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "查询题目失败\n" + err.Error()})
	} else {
		context.AbortWithStatusJSON(http.StatusOK, problemInfo)
	}
}

func deleteProblem(context *gin.Context) {
	id, err := strconv.Atoi(context.Param("ID"))
	if err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
		return
	}
	if err = db.DeleteProblem(id); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "删除题目失败\n" + err.Error()})
		return
	}
	context.AbortWithStatus(http.StatusOK)
}

func updateProblem(context *gin.Context) {
	var problemData model.Problem
	id, err := strconv.Atoi(context.Param("ID"))
	if err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
		return
	}
	problemData.ID = id
	if err := context.ShouldBindJSON(&problemData); err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
		return
	}
	if err := db.UpdateProblem(problemData); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "更新题目失败\n" + err.Error()})
		return
	}
	context.AbortWithStatus(http.StatusCreated)
}

func createProblem(context *gin.Context) {
	var problemData model.Problem
	if err := context.ShouldBindJSON(&problemData); err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
		return
	}
	if err := db.CreateProblem(problemData); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "创建题目失败\n" + err.Error()})
		return
	}
	context.AbortWithStatus(http.StatusCreated)
}

func getProblemList(context *gin.Context) {
	var problemData model.GetProblemDTO
	if err := context.ShouldBindQuery(&problemData); err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": err.Error()})
	}
	if problems, err := db.GetProblemByPage(problemData); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "查询题目失败\n" + err.Error()})
	} else {
		context.AbortWithStatusJSON(http.StatusOK, problems)
	}
}

func getProblemByID(context *gin.Context) {
	id, err := strconv.Atoi(context.Param("ID"))
	if err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
		return
	}
	if problemInfo, err := db.GetProblemById(id); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "查询题目失败\n" + err.Error()})
	} else {
		context.AbortWithStatusJSON(http.StatusOK, problemInfo)
	}
}

func updateNew(context *gin.Context) {
	var newData model.New
	id, err := strconv.Atoi(context.Param("ID"))
	if err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
		return
	}
	newData.ID = id
	newData.Author = context.MustGet("username").(string)
	newData.Time = time.Now().Format("2006-01-02 15:04:05")
	if err := context.ShouldBindJSON(&newData); err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
	}
	if err := db.UpdateNew(newData); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "更新新闻失败\n" + err.Error()})
	}
	context.AbortWithStatus(http.StatusCreated)
}

func getNewById(context *gin.Context) {
	id, err := strconv.Atoi(context.Param("ID"))
	if err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
		return
	}
	if newInfo, err := db.GetNewById(id); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "查询新闻失败\n" + err.Error()})
	} else {
		context.AbortWithStatusJSON(http.StatusOK, newInfo)
	}
}

func getNewList(context *gin.Context) {
	var newData model.GetNewDTO
	if err := context.ShouldBindQuery(&newData); err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": err.Error()})
	}
	if news, err := db.GetNewsByPage(newData); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "查询新闻失败\n" + err.Error()})
	} else {
		context.AbortWithStatusJSON(http.StatusOK, news)
	}
}

func deleteNew(context *gin.Context) {
	fmt.Println("run")
	id, err := strconv.Atoi(context.Param("ID"))
	if err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
		return
	}
	if err := db.DeleteNew(id); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "查询新闻失败\n" + err.Error()})
	} else {
		context.AbortWithStatus(http.StatusOK)
	}
}

func createNew(context *gin.Context) {
	var newData model.New
	newData.Author = context.MustGet("username").(string)
	newData.Time = time.Now().Format("2006-01-02 15:04:05")
	if err := context.ShouldBindJSON(&newData); err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
	}
	if err := db.CreateNew(newData); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "创建新闻失败\n" + err.Error()})
	}
	context.AbortWithStatus(http.StatusCreated)
}

func getInfoGroup(context *gin.Context) {
	username := context.MustGet("username").(string)
	if inviteInfo, err := db.GetInviteInfo(username); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "获取邀请信息失败\n" + err.Error()})
	} else {
		context.AbortWithStatusJSON(http.StatusOK, inviteInfo)
	}
}

func operateGroup(context *gin.Context) {
	//读取json的内容
	var approveData model.InviteGroupMessageDTO
	if err := context.ShouldBindJSON(&approveData); err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
	}
	approveData.InviterName = context.MustGet("username").(string)
	if err := db.OperateGroup(approveData); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "批准失败\n" + err.Error()})
	}
	context.AbortWithStatus(http.StatusOK)
}

// 随机生成一个密钥，用于签名和验证JWT
var jwtSecret = []byte("my-secret-key")

func AuthMiddlewareAdmin() gin.HandlerFunc {
	return func(c *gin.Context) {
		authHeader := c.GetHeader("Authorization")
		if authHeader == "" {
			c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"error": "您没有携带token令牌，请重新登录"})
		}
		parts := strings.Split(authHeader, " ")
		if len(parts) != 2 || parts[0] != "Bearer" {
			c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"error": "Authorization的格式错误，请重新登录"})
		}
		tokenString := parts[1]
		token, err := jwt.Parse(tokenString, func(token *jwt.Token) (interface{}, error) {
			if _, ok := token.Method.(*jwt.SigningMethodHMAC); !ok {
				return nil, jwt.ErrSignatureInvalid
			}
			return jwtSecret, nil
		})
		if err != nil || !token.Valid {
			c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"error": "token过期或不可用\n" + err.Error()})
		}
		if claims, ok := token.Claims.(jwt.MapClaims); ok && token.Valid {
			c.Set("username", claims["username"]) // 假设Token包含username
			c.Set("role", claims["role"])         // 假设Token包含role
		} else if claims["username"] == nil {
			c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"error": "token中缺少用户名"})
		} else if claims["role"] == false || claims["role"] == nil {
			c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"error": "权限不足"})
		}
		c.Next()
	}
}

func AuthMiddleware() gin.HandlerFunc {
	return func(c *gin.Context) {
		authHeader := c.GetHeader("Authorization")
		if authHeader == "" {
			c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"error": "Authorization缺失header"})
		}
		parts := strings.Split(authHeader, " ")
		if len(parts) != 2 || parts[0] != "Bearer" {
			c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"error": "Authorization的格式错误"})
		}
		tokenString := parts[1]
		token, err := jwt.Parse(tokenString, func(token *jwt.Token) (interface{}, error) {
			if _, ok := token.Method.(*jwt.SigningMethodHMAC); !ok {
				return nil, jwt.ErrSignatureInvalid
			}
			return jwtSecret, nil
		})

		if err != nil || !token.Valid {
			c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"error": "token过期或不可用\n" + err.Error()})
		}
		if claims, ok := token.Claims.(jwt.MapClaims); ok && token.Valid {
			c.Set("username", claims["username"]) // 假设Token包含username
			c.Set("role", claims["role"])         // 假设Token包含role
		} else if claims["username"] == nil {
			c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"error": "token中缺少用户名"})
		}
		c.Next()
	}
}

func GenerateToken(userID string, userRole bool) (string, error) {
	// 创建Token，设置bearer和claims
	token := jwt.NewWithClaims(jwt.SigningMethodHS256, jwt.MapClaims{
		"username": userID,
		"role":     userRole,
		"exp":      time.Now().Add(time.Hour * 7 * 24).Unix(), // 设置过期时间为7*24小时
	})
	// 签名Token
	tokenString, err := token.SignedString(jwtSecret)
	if err != nil {
		return "", err
	}
	return tokenString, nil
}

func getGroupById(context *gin.Context) {
	username := context.MustGet("username").(string)
	groupName := context.Param("groupName")
	if public, err := db.IsPublicGroup(groupName); err != nil {
		context.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"error": err.Error()})
		return
	} else if !public {
		context.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"error": "该群组是私密的"})
		return
	}
	if group, GroupMember, err := db.GroupInfo(groupName, username); err != nil {
		context.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"error": err.Error()})
		return
	} else {
		context.AbortWithStatusJSON(http.StatusOK, gin.H{"group": group, "member": GroupMember})
	}
}

func getGroupList(context *gin.Context) {
	var groupData model.RequestGroupDTO
	groupData.UserName = context.GetString("username")
	if err := context.ShouldBindQuery(&groupData); err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}
	groups, err := db.GetGroup(groupData)
	if err != nil {
		if errors.Is(err, gorm.ErrRecordNotFound) {
			context.AbortWithStatusJSON(http.StatusNotFound, gin.H{"error": "没有找到群组"})
			return
		}
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "查询群组失败\n" + err.Error()})
		return
	}
	context.AbortWithStatusJSON(http.StatusOK, groups)
}

func inviteGroup(context *gin.Context) {
	var inviteData model.InviteGroupDTO
	inviteData.GroupName = context.Param("groupName")
	inviteData.InviterName = context.MustGet("username").(string)
	if err := context.ShouldBindJSON(&inviteData); err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
	}
	if err := db.InviteGroup(inviteData); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "邀请失败\n" + err.Error()})
	}
	context.AbortWithStatus(http.StatusCreated)
}

func applyGroup(context *gin.Context) {
	var ApplyData model.ApplyGroupMessage
	ApplyData.GroupName = context.Param("groupName")
	ApplyData.Username = context.MustGet("username").(string)
	//写入数据库
	if err := db.ApplyGroup(ApplyData); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "申请失败\n" + err.Error()})
	}
}

func createGroup(context *gin.Context) {
	var data model.Group
	data.Owner = context.MustGet("username").(string)
	if err := context.ShouldBindJSON(&data); err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}
	if err := db.CreateGroup(data); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "创建群组失败\n" + err.Error()})
		return
	}
	context.AbortWithStatus(http.StatusCreated)
}

func home(context *gin.Context) {
	var newData model.GetNewDTO
	if err := context.ShouldBindQuery(&newData); err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": err.Error()})
	}
	if data, err := db.GetHomeData(newData); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "查询主页失败\n" + err.Error()})
	} else {
		context.AbortWithStatusJSON(http.StatusOK, data)
	}
}

func login(context *gin.Context) {
	// 1. 获取请求参数DTO
	var loginData model.LoginDTO
	if err := context.ShouldBindJSON(&loginData); err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
	}
	if user, err := db.Login(loginData); err != nil {
		if err.Error() == "record not found" {
			context.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"error": "用户名或密码错误"})
		} else {
			context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "登录失败" + err.Error()})
		}
	} else {
		if token, err := GenerateToken(user.Name, user.Role); err != nil {
			context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "生成Token失败" + err.Error()})
		} else {
			err = db.SetVerifyCode(user.Name, token)
			if err != nil {
				context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "存储Token失败" + err.Error()})
			} else {
				context.AbortWithStatusJSON(http.StatusOK, gin.H{"token": token})
			}
		}
	}
}

func register(context *gin.Context) {
	var registerData model.RegisterDTO
	if err := context.ShouldBindJSON(&registerData); err != nil {
		context.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
	}
	if verifyCode, err := db.GetVerifyCode(registerData.Email); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "获取验证码失败" + err.Error()})
	} else if verifyCode != registerData.Code {
		context.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"error": "验证码错误"})
	}
	if user, err := db.Register(registerData.Username, registerData.Password, registerData.Email); err != nil {
		context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "创建用户失败\n" + err.Error()})
	} else {
		if token, err := GenerateToken(user.Name, user.Role); err != nil {
			context.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"error": "生成Token失败" + err.Error()})
		} else {
			context.AbortWithStatusJSON(http.StatusOK, gin.H{"token": token})
		}
	}
}

func sendMail(context *gin.Context) {
	//查询
	var emailData model.SendEmailDTO
	if err := context.ShouldBindJSON(&emailData); err != nil {
		context.JSON(http.StatusBadRequest, gin.H{"error": "请求参数错误"})
		return
	}
	//生成邮箱验证码
	verifyCode := fmt.Sprintf("%06d", rand.Intn(1000000))
	//发送邮件
	hostMail := "smtp.qq.com"
	port := 465
	username := "1009422458@qq.com"
	password := "kjfsxlkeuqdpbfbe"
	d := gomail.NewDialer(
		hostMail,
		port,
		username,
		password)
	d.TLSConfig = &tls.Config{InsecureSkipVerify: true}
	m := gomail.NewMessage()
	m.SetHeader("From", username)
	m.SetHeader("To", emailData.Email)
	m.SetBody("text/plain", "Online_Judge的邮箱验证码为:"+verifyCode+"    （有效期2小时）")
	//发送邮件
	if err := d.DialAndSend(m); err != nil {
		context.JSON(500, gin.H{"error": "发送失败" + err.Error()})
	}
	//把(key:username,value:邮箱的验证码)存到Redis里面
	if err := db.SetVerifyCode(emailData.Email, verifyCode); err != nil {
		context.JSON(500, gin.H{
			"message": "存储验证码失败" + err.Error(),
		})
		return
	}
	context.JSON(http.StatusOK, gin.H{"message": "发送成功，请注意查收，有效期2小时"})
}

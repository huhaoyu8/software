package db

import (
	"back/model"
	"errors"
	"fmt"
	"gorm.io/driver/mysql"
	"gorm.io/gorm"
	"log"
	"time"
)

var DB *gorm.DB

func InitDB() {
	var err error
	dsn := "root:huhaoyu@tcp(localhost:3306)/cugoj?charset=utf8"
	DB, err = gorm.Open(mysql.Open(dsn), &gorm.Config{})

	if err != nil {
		log.Fatalf("数据库连接失败： %v", err)
	}
	//删除所有table
	err = DB.Migrator().DropTable(&model.User{}, &model.New{}, &model.Problem{}, &model.CompileInfo{},
		&model.Contest{}, &model.Group{}, &model.GroupMember{}, &model.Rank{}, &model.Url{}, &model.Url{},
		&model.InviteGroupMessage{}, &model.ApplyGroupMessage{}, &model.ContestProblem{})
	//创建表
	err = DB.AutoMigrate(&model.User{}, &model.New{}, &model.Problem{}, &model.CompileInfo{},
		&model.Contest{}, &model.Group{}, &model.GroupMember{}, &model.Rank{}, &model.Url{}, &model.Url{},
		&model.InviteGroupMessage{}, &model.ApplyGroupMessage{}, &model.ContestProblem{})
	addTestData()
	if err != nil {
		log.Fatalf("数据库迁移失败： %v", err)
	}
}

// --------------------------主页-------------------
func GetHomeData(newData model.GetNewDTO) (model.HomeDTO, error) {
	var homeData model.HomeDTO
	//获取排行榜
	ranks, err := GetRank()
	if err != nil {
		return model.HomeDTO{}, err
	}
	homeData.RankList = ranks

	//获取Url
	var urls []model.Url
	err = DB.Find(&urls).Error
	if err != nil {
		return model.HomeDTO{}, err
	}
	homeData.Url = urls

	//获取新闻
	news, err := GetNewsByPage(newData)
	if err != nil {
		return model.HomeDTO{}, err
	}
	homeData.NewList = news

	return homeData, nil
}

//--------------------------获取排行榜-------------------

func GetRank() ([]model.Rank, error) {
	var ranks []model.Rank
	//选取前10名用户
	err := DB.Model(&model.Rank{}).
		Select("username, top").
		Order("top ASC").
		Limit(10).
		Find(&ranks).Error
	if err != nil {
		return nil, err
	}
	return ranks, nil
}

// ------------------------测试--------------------
func addTestData() {
	//添加测试数据
	user := model.User{Name: "胡浩宇", Password: "2", Role: true, Email: "test@test.com"}
	err := DB.Create(&user).Error
	user = model.User{Name: "张三", Password: "2", Role: true, Email: "test@test.com"}
	err = DB.Create(&user).Error
	user = model.User{Name: "李四", Password: "2", Role: true, Email: "test@test.com"}
	err = DB.Create(&user).Error
	user = model.User{Name: "王五", Password: "2", Role: true, Email: "test@test.com"}
	err = DB.Create(&user).Error
	user = model.User{Name: "小明", Password: "2", Role: true, Email: "test@test.com"}
	err = DB.Create(&user).Error
	user = model.User{Name: "小红", Password: "2", Role: true, Email: "test@test.com"}
	err = DB.Create(&user).Error
	for i := 0; i < 5; i++ {
		New := model.New{Time: time.Date(2025, 5, 15, 15, 4, 5, 0, time.UTC).Format("2006-01-02 15:04:05"),
			Title: "第" + fmt.Sprint(i+1) + "条新闻标题", Author: "张三", Importance: true, Content: "　“一项法案要么宏大，要么美丽，但不确定两者是否能够兼得。”在告别一群被认为世界上最有权势的同僚之前，埃隆·马斯克用嘲笑给予最后一击。\n\n　　马斯克批评的是美国总统特朗普称为“美丽大法案”的一份税收与减支法案，这份体现特朗普竞选承诺、号称让美国再次伟大的2.0税改议案，于5月21日在众议院惊险通过，它还将接受参议院的考验。特朗普将此视为胜利，但作为总统亲密无间的盟友，马斯克认为，法案不仅未能遏制预算赤字，反而可能进一步扩大财政缺口，这与其领导的政府效率部所倡导的改革方向背道而驰。\n\n"}
		err = DB.Create(&New).Error
	}
	//创建group
	group := model.Group{
		Name:        "群组名称",
		Owner:       "胡浩宇",
		Description: "这是一个测试群组，描述了群组的基本信息，",
		Visibility:  true,
		//指定时间2025-05-15 15:04:05
		CreateTime: time.Date(2025, 5, 15, 15, 4, 5, 0, time.UTC).Format("2006-01-02 15:04:05"),
	}
	err = DB.Create(&group).Error
	//创建groupMember
	groupMember := model.GroupMember{
		GroupName: "群组名称",
		Role:      2, //群主
		Username:  "胡浩宇",
		JoinTime:  time.Date(2025, 5, 15, 15, 4, 5, 0, time.UTC).Format("2006-01-02 15:04:05"),
	}
	err = DB.Create(&groupMember).Error
	groupMember = model.GroupMember{
		GroupName: "群组名称",
		Role:      0, //成员
		Username:  "张三",
		JoinTime:  time.Date(2025, 5, 15, 15, 4, 5, 0, time.UTC).Format("2006-01-02 15:04:05"),
	}
	err = DB.Create(&groupMember).Error
	//创建invite
	inviteGroupMessage := model.InviteGroupMessage{
		GroupName:   "群组名称",
		InviterName: "张三",
		Username:    "胡浩宇",
		UserAgree:   true,
	}
	err = DB.Create(&inviteGroupMessage).Error
	problem := model.Problem{
		ID:            1,
		Title:         "A+B problem",
		TimeLimit:     1,
		MemoryLimit:   1,
		Description:   "test",
		Input:         "1 2",
		Output:        "3",
		SampleInput:   "1 2",
		SampleOutput:  "3",
		Hint:          "A+B",
		Source:        "test",
		Solution:      "test",
		Difficulty:    1,
		SubmitCount:   79,
		AcceptedCount: 43,
		Tag:           "test",
	}
	err = DB.Create(&problem).Error
	contest := model.Contest{
		ID:       1,
		Title:    "比赛的名称",
		Begin:    time.Date(2025, 5, 15, 15, 4, 5, 0, time.UTC).Format("2006-01-02 15:04:05"),
		End:      time.Date(2025, 10, 15, 15, 4, 5, 0, time.UTC).Format("2006-01-02 15:04:05"),
		Password: "test",
		Description: "比赛形式是团体积分赛。每个参赛团队最多由3名参赛选手组成。各队参赛选手独立作答，赛后综合3名选手成绩进行团队成绩排名。参赛团队在指定机器上进行实时答题，题目数量是8~10题。" +
			"比赛期间需根据要求利用C语言、C++语言进行编程答题，提交源代码。" +
			"源代码将由计算机评判系统自动评判。赛后，将对获奖选手的代码进行人工审查，对于确认抄袭的选手，将取消获奖资格。",
	}
	err = DB.Create(&contest).Error
	contestProblem := model.ContestProblem{
		ContestID:     1,
		ID:            10,
		Title:         "A+B problem",
		TimeLimit:     1,
		MemoryLimit:   1,
		Description:   "test",
		Input:         "1 2",
		Output:        "3",
		SampleInput:   "1 2",
		SampleOutput:  "3",
		Hint:          "A+B",
		Source:        "test",
		Solution:      "test",
		Difficulty:    1,
		SubmitCount:   79,
		AcceptedCount: 43,
		Tag:           "test",
	}
	err = DB.Create(&contestProblem).Error
	rank := model.Rank{
		Username: "胡浩宇",
		Top:      1,
	}
	err = DB.Create(&rank).Error
	rank = model.Rank{
		Username: "张三",
		Top:      2,
	}
	err = DB.Create(&rank).Error
	rank = model.Rank{
		Username: "李四",
		Top:      3,
	}
	err = DB.Create(&rank).Error
	rank = model.Rank{
		Username: "王五",
		Top:      4,
	}
	err = DB.Create(&rank).Error
	rank = model.Rank{
		Username: "小明",
		Top:      5,
	}
	err = DB.Create(&rank).Error
	url := model.Url{
		Url: "https://fastly.picsum.photos/id/802/800/450.jpg?hmac=k7WqhqJvADK3_NSrHvVZziUA3s8QPcONPf60NPFX86k",
	}
	err = DB.Create(&url).Error
	url = model.Url{
		Url: "https://fastly.picsum.photos/id/802/800/450.jpg?hmac=k7WqhqJvADK3_NSrHvVZziUA3s8QPcONPf60NPFX86k",
	}
	err = DB.Create(&url).Error
	url = model.Url{
		Url: "https://fastly.picsum.photos/id/802/800/450.jpg?hmac=k7WqhqJvADK3_NSrHvVZziUA3s8QPcONPf60NPFX86k",
	}
	err = DB.Create(&url).Error
	url = model.Url{
		Url: "https://fastly.picsum.photos/id/802/800/450.jpg?hmac=k7WqhqJvADK3_NSrHvVZziUA3s8QPcONPf60NPFX86k",
	}
	err = DB.Create(&url).Error
	if err != nil {
		log.Fatalf("添加测试数据失败： %v", err)
	}
}

// ------------------------用户-------------------
func Login(loginData model.LoginDTO) (model.User, error) {
	var user model.User
	err := DB.Where("name = ? and password = ?", loginData.Username, loginData.Password).First(&user).Error
	if err != nil {
		return model.User{}, err
	}
	return user, nil
}

func Register(username string, password string, email string) (model.User, error) {
	user := model.User{Name: username, Password: password, Email: email, Role: false}
	err := DB.Create(&user).Error
	if err != nil {
		return model.User{}, err
	}
	return user, nil
}

// --------------------------竞赛-------------------
func GetContestList(contestData model.GetContestDTO, admin bool) ([]model.Contest, error) {
	var contests []model.Contest
	if admin {
		err := DB.
			Model(&model.Contest{}).
			Offset((contestData.Page - 1) * contestData.PageSize).
			Select("id, title, begin, end, password").
			Limit(contestData.PageSize).
			Find(&contests).Error
		if err != nil {
			return nil, err
		}
		return contests, nil
	}
	err := DB.
		Model(&model.Contest{}).
		Offset((contestData.Page - 1) * contestData.PageSize).
		Select("id, title, begin, end").
		Limit(contestData.PageSize).
		Find(&contests).Error
	if err != nil {
		return nil, err
	}
	return contests, nil
}

func GetContestById(id int, admin bool) (model.Contest, error) {
	var contest model.Contest
	if admin {
		err := DB.
			Model(model.Contest{}).
			Where("id = ?", id).
			Select("id, title, begin, end, password,Description").
			First(&contest).Error
		if err != nil {
			return model.Contest{}, err
		}
		return contest, nil
	}
	err := DB.
		Model(model.Contest{}).
		Where("id = ?", id).
		Select("id, title, begin, end,Description").
		First(&contest).Error
	if err != nil {
		return model.Contest{}, err
	}
	return contest, nil
}

func CreateContest(contestData model.Contest) error {
	err := DB.Create(&contestData).Error
	if err != nil {
		return err
	}
	return nil
}

func DeleteContest(id int) error {
	err := DB.Where("id = ?", id).Delete(&model.Contest{}).Error
	if err != nil {
		return err
	}
	return nil
}

func UpdateContest(contestData model.Contest) error {
	err := DB.
		Model(&model.Contest{}).
		Updates(&contestData).
		Where("id = ?", contestData.ID).Error
	if err != nil {
		return err
	}
	return nil
}

func GetContestProblemList(id int) ([]model.ContestProblem, error) {
	var contestProblems []model.ContestProblem
	err := DB.Model(&model.ContestProblem{}).Where("contest_id = ?", id).Find(&contestProblems).Error
	if err != nil {
		return nil, err
	}
	return contestProblems, nil
}

func GetContestProblemById(id int) (model.ContestProblemDetailDTO, error) {
	var problem model.ContestProblemDetailDTO
	err := DB.Model(model.ContestProblem{}).Where("id = ?", id).First(&problem).Error
	if err != nil {
		return model.ContestProblemDetailDTO{}, err
	}
	return problem, nil
}

// --------------------------题目--------------------
func GetProblemByPage(problemData model.GetProblemDTO) ([]model.ProblemListDTO, error) {
	var problems []model.ProblemListDTO
	err := DB.
		Model(&model.Problem{}).
		Offset((problemData.Page - 1) * problemData.PageSize).
		Select("id, title, accepted_count, submit_count, tag, difficulty").
		Limit(problemData.PageSize).
		Find(&problems).Error
	if err != nil {
		return nil, err
	}
	return problems, nil
}

func GetProblemById(id int) (model.ProblemDetailDTO, error) {
	var problem model.ProblemDetailDTO
	err := DB.Model(model.Problem{}).Where("id = ?", id).First(&problem).Error
	if err != nil {
		return model.ProblemDetailDTO{}, err
	}
	return problem, nil
}

func CreateProblem(problemData model.Problem) error {
	err := DB.Create(&problemData).Error
	if err != nil {
		return err
	}
	return nil
}

func DeleteProblem(id int) error {
	err := DB.Where("id = ?", id).Delete(&model.Problem{}).Error
	if err != nil {
		return err
	}
	return nil
}

func UpdateProblem(problemData model.Problem) error {
	err := DB.
		Model(&model.Problem{}).
		Updates(&problemData).
		Where("id = ?", problemData.ID).Error
	if err != nil {
		return err
	}
	return nil
}

// -------------------------新闻-------------------

func GetNewsByPage(newData model.GetNewDTO) ([]model.New, error) {
	var news []model.New
	err := DB.Offset((newData.Page-1)*newData.PageSize).
		Select("time, title, importance,author", "id").
		Limit(newData.PageSize).
		Find(&news).Error
	if err != nil {
		return nil, err
	}
	return news, nil
}

func GetNewById(id int) (model.New, error) {
	var newData model.New
	err := DB.Where("id = ?", id).First(&newData).Error
	if err != nil {
		return model.New{}, err
	}
	return newData, nil
}

func CreateNew(newData model.New) error {
	if err := DB.Create(&newData).Error; err != nil {
		return err
	}
	return nil
}

func DeleteNew(id int) error {
	err := DB.Where("id = ?", id).Delete(&model.New{}).Error
	if err != nil {
		return err
	}
	return nil
}

func UpdateNew(newData model.New) error {
	err := DB.
		Where("id = ?", newData.ID).
		Updates(&newData).Error
	if err != nil {
		return err
	}
	return nil
}

// -------------------------群组-------------------
func CreateGroup(groupData model.Group) error {
	//Group表
	group := model.Group{
		Name:        groupData.Name,
		Owner:       groupData.Owner,
		Description: groupData.Description,
		CreateTime:  time.Now().Format("2006-01-02 15:04:05"),
		Visibility:  groupData.Visibility,
	}
	if err := DB.Create(&group).Error; err != nil {
		return err
	}
	//Member表
	groupMember := model.GroupMember{
		GroupName: groupData.Name,
		Role:      2, //群主
		Username:  groupData.Owner,
		JoinTime:  time.Now().Format("2006-01-02 15:04:05"),
	}
	if err := DB.Create(&groupMember).Error; err != nil {
		return err
	}
	return nil
}

func GetGroup(groupData model.RequestGroupDTO) ([]model.Group, error) {
	//	查询所有visible的群组
	if groupData.GroupName == "" {
		var groups []model.Group
		err := DB.Model(&model.Group{}).
			Offset((groupData.Page-1)*groupData.PageSize).
			Where("visibility = 1 and name LIKE ?", "%"+groupData.GroupName+"%").
			Select("name", "owner", "Description", "CreateTime").
			Find(&groups).Error
		if err != nil {
			return nil, err
		}
		return groups, nil
	} else {
		var groups []model.Group
		err := DB.
			Model(&model.Group{}).
			Where("group_members.username = ?", groupData.UserName).
			Joins("JOIN group_members ON group_members.group_name = groups.name").
			Select("groups.name, groups.owner, groups.description, groups.create_time").
			Scan(&groups).Error
		if err != nil {
			return nil, err
		}
		return groups, nil
	}
}

func IsPublicGroup(groupName string) (bool, error) {
	var group model.Group
	err := DB.Where("name = ?", groupName).First(&group).Error
	if err != nil {
		return false, err
	}
	return group.Visibility, nil
}

func IsGroupMember(username string, groupName string) (bool, error) {
	var exist bool
	err := DB.Model(&model.GroupMember{}).
		Where("username = ? and group_name = ?", username, groupName).
		Select("count(*) > 0").
		Find(&exist).Error
	if err != nil {
		return false, err
	} else if !exist {
		return false, nil
	}
	return true, nil
}

func IsGroupAdmin(username string, groupName string) (bool, error) {
	var groupMember model.GroupMember
	err := DB.Where("username = ? and group_name = ? and role = ?", username, groupName, true).First(&groupMember).Error
	if err != nil {
		return false, err
	}
	return true, nil
}

func GroupInfo(groupName string, username string) (model.Group, []model.GroupMember, error) {
	if public, err := IsPublicGroup(groupName); err != nil || public == false {
		return model.Group{}, nil, err
	}
	if member, err := IsGroupMember(username, groupName); err != nil || member == false {
		return model.Group{}, nil, err
	}
	var group model.Group
	err := DB.
		Where("name = ?", groupName).
		Model(&model.Group{}).
		Select("name", "owner", "description", "create_time").
		First(&group).Error
	if err != nil {
		return model.Group{}, nil, err
	}
	var groupMembers []model.GroupMember
	err = DB.
		Model(&model.GroupMember{}).
		Where("group_name = ?", groupName).
		Select("role", "username", "join_time").
		Scan(&groupMembers).Error
	if err != nil {
		return model.Group{}, nil, err
	}
	fmt.Println("groupMembers:", groupMembers)
	return group, groupMembers, nil
}

func InviteGroup(inviteData model.InviteGroupDTO) error {
	if isAdmin, err := IsGroupAdmin(inviteData.InviterName, inviteData.GroupName); err != nil || !isAdmin {
		return err
	}
	for _, username := range inviteData.Username {
		inviteMessage := model.InviteGroupMessage{
			GroupName:   inviteData.GroupName,
			InviterName: inviteData.InviterName,
			Username:    username,
			UserAgree:   false,
		}
		err := DB.Create(&inviteMessage).Error
		if err != nil {
			return err
		}
	}
	return nil
}

func GetInviteInfo(InviterName string) ([]model.InviteGroupMessageDTO, error) {
	var inviteMessages []model.InviteGroupMessageDTO
	err := DB.
		Model(&model.InviteGroupMessage{}).
		Where("inviter_name = ? and user_agree=?", InviterName, true).
		Select("group_name", "username").
		Find(&inviteMessages).Error
	if err != nil {
		return nil, err
	} else {
		return inviteMessages, nil
	}
}

func ApplyGroup(applyData model.ApplyGroupMessage) error {
	if isMember, err := IsGroupMember(applyData.Username, applyData.GroupName); err != nil || isMember {
		if err != nil {
			return errors.New("已经是群组成员")
		}
		return err
	}
	var OwnerName string
	err := DB.Model(&model.Group{}).
		Where("group_name = ?", applyData.GroupName).
		Select("username").
		First(&OwnerName).Error
	if err != nil {
		return err
	}
	applyMessage := model.ApplyGroupMessage{
		GroupName: applyData.GroupName,
		Username:  applyData.Username,
	}
	if err = DB.Create(&applyMessage).Error; err != nil {
		return err
	}
	return nil
}

func OperateGroup(inviteData model.InviteGroupMessageDTO) error {
	if inviteData.InviterAgree == false {
		if err := DB.
			Where("inviter_name = ? and user_agree = ? and group_name = ? and username = ?",
				inviteData.InviterName, true, inviteData.GroupName, inviteData.Username).
			Delete(&model.InviteGroupMessage{}).Error; err != nil {
			return err
		}
		return nil
	}
	if isMember, err := IsGroupMember(inviteData.Username, inviteData.GroupName); err != nil {
		return err
	} else if isMember {
		return errors.New("申请人已经成为群组成员")
	}
	var exist bool
	err := DB.Model(&model.InviteGroupMessage{}).
		Where("inviter_name = ? and user_agree = ? and group_name = ? and username = ?", inviteData.InviterName, true, inviteData.GroupName, inviteData.Username).
		Select("count(*) > 0").
		Find(&exist).Error
	if err != nil || !exist {
		if !exist {
			return errors.New("你通过了一条不存在的空申请")
		}
		return err
	}
	groupMember := model.GroupMember{
		GroupName: inviteData.GroupName,
		Role:      0,
		Username:  inviteData.Username,
		JoinTime:  time.Now().Format("2006-01-02 15:04:05"),
	}
	if err = DB.Model(&model.GroupMember{}).Create(&groupMember).Error; err != nil {
		return err
	}
	if err = DB.
		Where("inviter_name = ? and user_agree = ? and group_name = ? and username = ?",
			inviteData.InviterName, true, inviteData.GroupName, inviteData.Username).
		Delete(&model.InviteGroupMessage{}).Error; err != nil {
		return err
	}
	return nil
}

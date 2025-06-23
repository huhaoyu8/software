package model

type HomeDTO struct {
	RankList []Rank `json:"rankList"`
	Url      []Url  `json:"url"`
	NewList  []New  `json:"newList"`
}

// -------------竞赛--------------------
type GetContestDTO struct {
	Page     int `form:"page"`
	PageSize int `form:"pageSize"`
}

// -------------题目------------------
type GetProblemDTO struct {
	Page     int `form:"page"`
	PageSize int `form:"pageSize"`
}

type ProblemListDTO struct {
	ID            int    `json:"id"`
	Title         string `json:"title"`
	AcceptedCount uint   `json:"acceptedCount"`
	SubmitCount   uint   `json:"submitCount"`
	Tag           string `json:"tag"`
	Difficulty    uint8  `json:"difficulty"`
}

type ProblemDetailDTO struct {
	ID            int    `gorm:"primaryKey" json:"id"`
	Title         string `json:"title"`
	TimeLimit     uint   `json:"timeLimit"`   //时间限制,毫秒ms
	MemoryLimit   uint   `json:"memoryLimit"` //内存限制,MB
	Description   string `json:"description"`
	Input         string `json:"input"`        //文字形式
	Output        string `json:"output"`       //文字形式
	SampleInput   string `json:"sampleInput"`  //数据形式
	SampleOutput  string `json:"sampleOutput"` //数据形式
	Hint          string `json:"hint"`
	Source        string `json:"source"`
	Difficulty    uint8  `json:"difficulty"`
	SubmitCount   uint   `json:"submitCount"`
	AcceptedCount uint   `json:"acceptedCount"`
	Tag           string `json:"tag"`
}

type ContestProblemDetailDTO struct {
	ID            int    `gorm:"primaryKey" json:"id"`
	Title         string `json:"title"`
	TimeLimit     uint   `json:"timeLimit"`   //时间限制,毫秒ms
	MemoryLimit   uint   `json:"memoryLimit"` //内存限制,MB
	Description   string `json:"description"`
	Input         string `json:"input"`        //文字形式
	Output        string `json:"output"`       //文字形式
	SampleInput   string `json:"sampleInput"`  //数据形式
	SampleOutput  string `json:"sampleOutput"` //数据形式
	Hint          string `json:"hint"`
	Source        string `json:"source"`
	Difficulty    uint8  `json:"difficulty"`
	SubmitCount   uint   `json:"submitCount"`
	AcceptedCount uint   `json:"acceptedCount"`
	Tag           string `json:"tag"`
}

type GetNewDTO struct {
	Page     int `form:"page"`
	PageSize int `form:"pageSize"`
}
type LoginDTO struct {
	Username string `json:"username"`
	Password string `json:"password"`
}

type SendEmailDTO struct {
	Email string `json:"email"`
}

type RegisterDTO struct {
	Username string `json:"username"`
	Password string `json:"password"`
	Email    string `json:"email"`
	Code     string `json:"code"`
}

type RequestGroupDTO struct {
	GroupName string `form:"groupName"`
	Page      int    `form:"page" binding:"min=1"`
	PageSize  int    `form:"pageSize" binding:"min=1,max=20"`
	UserName  string
}

// -----------------群组------------------
type InviteGroupDTO struct {
	Username    []string `json:"username"`
	GroupName   string
	InviterName string
}

type InviteGroupMessageDTO struct {
	InviterName  string `json:"inviter_name"`
	Username     string `json:"username"`
	GroupName    string `json:"group_name"`
	UserAgree    bool
	InviterAgree bool `json:"inviter_agree"`
}

package model

type Log struct {
	CPU      float64 `gorm:"type:float" json:"cpu"`
	Mem      float64 `gorm:"type:float" json:"mem"`
	Disk     float64 `gorm:"type:float" json:"disk"`
	Recv     uint64  `gorm:"type:uint64" json:"recv"`
	Send     uint64  `gorm:"type:uint64" json:"send"`
	Load     float64 `gorm:"type:float" json:"load"`
	HostInfo string  `gorm:"type:text" json:"host_info"`
	CpuInfo  string  `gorm:"type:text" json:"cpu_info"`
}

type User struct {
	Name     string `gorm:"primaryKey;type:varchar(255);unique" json:"username"`
	Password string `json:"password"`
	Role     bool   `json:"role"`
	Email    string `json:"email"`
}

type Url struct {
	Url string `gorm:"type:text;" json:"url"`
}

type Rank struct {
	Username string `gorm:"primaryKey;type:varchar(255);unique" json:"username"`
	Top      int    `json:"top"`
	User     User   `gorm:"foreignKey:Username;references:Name"`
}

type New struct {
	ID         int    `gorm:"primaryKey" json:"id"`
	Time       string `json:"time"`
	Title      string `json:"title"`
	Author     string `gorm:"type:varchar(255);" json:"author"`
	Importance bool   `json:"importance"`
	Content    string `json:"content"`
	User       User   `gorm:"foreignKey:Author;references:Name"`
}

// 群组
type Group struct {
	Name        string `gorm:"primaryKey;type:varchar(255);unique" json:"name"`
	Owner       string `json:"owner"`
	Description string `json:"description"`
	Visibility  bool   `json:"visibility"`
	CreateTime  string `json:"create_time"`
	User        User   `gorm:"foreignKey:Owner;references:Name"`
}
type GroupMember struct {
	GroupName string ` json:"group_name"`
	Role      uint8  `json:"role"`
	Username  string `json:"username"`
	JoinTime  string `json:"join_time"`
	User      User   `gorm:"foreignKey:Username;references:Name"`
	Group     Group  `gorm:"foreignKey:GroupName;references:Name"`
}

type InviteGroupMessage struct {
	InviterName string `json:"inviter_name"`
	Username    string `json:"username"`
	GroupName   string `json:"group_name"`
	UserAgree   bool
	Group       Group `gorm:"foreignKey:GroupName;references:Name"`
	Inviter     User  `gorm:"foreignKey:InviterName;references:Name"`
	User        User  `gorm:"foreignKey:Username;references:Name"`
}

type ApplyGroupMessage struct {
	Username  string `json:"username"`
	GroupName string `json:"group_name"`
	OwnerName string
	Group     Group `gorm:"foreignKey:GroupName;references:Name"`
	Owner     User  `gorm:"foreignKey:OwnerName;references:Name"`
	User      User  `gorm:"foreignKey:Username;references:Name"`
}

type Problem struct {
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
	Solution      string `json:"solution"`
	Difficulty    uint8  `json:"difficulty"`
	SubmitCount   uint   `json:"submitCount"`
	AcceptedCount uint   `json:"acceptedCount"`
	Tag           string `json:"tag"`
}

type CompileInfo struct {
	SolutionID string `json:"solution_id"`
	CompileID  string `json:"compile_id"`
}

type Contest struct {
	ID          int    `gorm:"primaryKey" json:"id"`
	Title       string `json:"title"`
	Begin       string `json:"begin"`
	End         string `json:"end"`
	Password    string `json:"password"`
	Description string `json:"description"`
}

type ContestUser struct {
	ContestID int     `json:"contest_id"`
	Username  string  `json:"username"`
	Contest   Contest `gorm:"foreignKey:ContestID;references:ID"`
}

type ContestProblem struct {
	ContestID     int     `json:"contest_id"`
	ID            int     `gorm:"primaryKey" json:"id"`
	Title         string  `json:"title"`
	TimeLimit     uint    `json:"timeLimit"`   //时间限制,毫秒ms
	MemoryLimit   uint    `json:"memoryLimit"` //内存限制,MB
	Description   string  `json:"description"`
	Input         string  `json:"input"`        //文字形式
	Output        string  `json:"output"`       //文字形式
	SampleInput   string  `json:"sampleInput"`  //数据形式
	SampleOutput  string  `json:"sampleOutput"` //数据形式
	Hint          string  `json:"hint"`
	Source        string  `json:"source"`
	Solution      string  `json:"solution"`
	Difficulty    uint8   `json:"difficulty"`
	SubmitCount   uint    `json:"submitCount"`
	AcceptedCount uint    `json:"acceptedCount"`
	Tag           string  `json:"tag"`
	Contest       Contest `gorm:"foreignKey:ContestID;references:ID"`
}

type Status struct {
	UserID    string `json:"user_id"`
	DateTime  string `json:"date_time"`
	ProblemID string `json:"problem_id"`
	Language  string `json:"language"`
	Memory    int    `json:"memory"`
	Time      int    `json:"time"`
	Result    string `json:"result"`
}

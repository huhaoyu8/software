<script setup>
import {Search} from "@element-plus/icons-vue";
import {onMounted, reactive, ref} from 'vue'
import axios from "axios";
import {ElMessage} from "element-plus";
//组件的隐藏显示
const vision = ref(false)
const editVision = ref(false)
//返回值，查找操作
const total = defineModel('total', {default: 0})
const contestList = ref([]);
const form = reactive({
  id:'',
  title: '',
  begin:'',
  end:'',
  password:'',
  description: '',
})
const editForm = ref({
  id:'',
  title: '',
  begin:'',
  end:'',
  password:'',
  description: '',
})
//表单，查找操作
const readForm = reactive({
  value: '',
  type: '',
  page: 1,
  pageSize: 10,
})

//页面加载时获取信息
onMounted(() => {
  getContestList()
})

function getContestById(id) {
  axios.get('/api/contest/'+id, {
    headers: {
      'Authorization': "Bearer " + localStorage.getItem('token'),
    }
  }).then(res => {
    editForm.value = res.data;
  }).catch(err => {
    ElMessage.error(err.response.data);
  })
}

//增加操作HTTP
function create() {
  axios.post("/api/contest", form, {
    headers: {
      'Authorization': "Bearer " + localStorage.getItem('token'),
      'Content-Type': "application/json"
    }
  }).then(() => {
    getContestList()
    vision.value = false
    form.id=''
    form.title = ''
    form.begin = ''
    form.end = ''
    form.password = ''
    form.description = ''
    ElMessage.success("添加成功")
  }).catch(err => {
    ElMessage.error(err.response.data);
  })
}

//删除操作HTTP
function remove(id){
  axios.delete('/api/contest/'+id, {
    headers: {
      'Authorization': "Bearer " + localStorage.getItem('token'),
    }
  }).then(() => {
    getContestList()
    ElMessage.success("删除成功")
  }).catch(err => {
    ElMessage.error(err.response.data);
  })
}

//修改操作HTTP
function update(id) {
  axios.put("/api/contest/"+id, editForm.value, {
    headers: {
      'Authorization': "Bearer " + localStorage.getItem('token'),
      'Content-Type': "application/json"
    }
  }).then(() => {
    getContestList()
    editVision.value = false
    editForm.id=''
    editForm.title = ''
    editForm.begin = ''
    editForm.end = ''
    editForm.password = ''
    editForm.description = ''
    ElMessage.success("修改成功")
  }).catch(err => {
    ElMessage.error(err.response.data);
  })
}

//查找操作HTTP
function getContestList() {
  axios.defaults.headers.common['Authorization'] = "Bearer " + localStorage.getItem('token');
  axios.get('/api/contest' + '?page=' + readForm.page + '&pageSize=' + readForm.pageSize
  ).then(res => {
    contestList.value = res.data;
    if (res.data.length === 0) {
      ElMessage.info('没有更多新闻了');
    }
  }).catch(err => {
    ElMessage.error(err.response.data);
  })
}

</script>

<template>
  <!--  搜索框-->
  <el-space style="float: right;margin-bottom: 20px">
    <el-button @click="vision=true;">添加</el-button>
    <el-input @input="getContestList" v-model="readForm.value" placeholder="输入数据搜索"
              style="width: 450px;height: auto;margin:auto 50px auto 50px">
      <template #prepend>
        <el-select @change="getContestList()" v-model="readForm.type" placeholder="搜索类型" style="width: 100px">
          <el-option label="新闻编号" value="ID"/>
          <el-option label="标题" value="title"/>
          <el-option label="作者" value="author"/>
          <el-option label="内容" value="content"/>
          <el-option label="更新时间" value="updateTime"/>
        </el-select>
      </template>
      <template #append>
        <el-button @click="getContestList()" :icon="Search"></el-button>
      </template>
    </el-input>
  </el-space>
  <!--  显示的表格-->
  <el-table max-height="80%" v-model:data="contestList" border style="width: 100%;margin-top:20px">
    <el-table-column prop="id" label="编号" width="80"/>
    <el-table-column prop="title" label="名称"/>
    <el-table-column prop="begin" label="开始时间"/>
    <el-table-column prop="end" label="结束时间"/>
    <el-table-column prop="password" label="密码"/>
    <el-table-column label="操作">
      <template #default="item">
        <el-button @click="remove(item.row.id)" link type="primary">删除</el-button>
        <el-button @click="getContestById(item.row.id);editVision=true" link type="primary">编辑</el-button>
        <el-button @click="this.$router.push('/contest/' + item.row.id)" link type="primary">详情</el-button>
      </template>
    </el-table-column>
  </el-table>
  <!--  分页组件-->
  <div style="position: absolute;bottom: 15px ">
    <el-pagination
        background
        @current-change="getContestList()"
        @size-change="getContestList()"
        :page-sizes="[8, 12, 50, 100]"
        v-model:current-page="readForm.page"
        v-model:page-size="readForm.pageSize"
        layout="sizes,prev, pager, next,jumper"
        :total="total"/>
  </div>
  <!--  窗口，添加操作-->
  <el-drawer v-model="vision" :center="true" title= "添加比赛" size="50%" :with-header="true">
    <el-form :model="form" label-width="auto">
      <el-form-item label="比赛名称">
        <el-input v-model="form.title" placeholder="请输入名称"></el-input>
      </el-form-item>
      <el-space>
        <el-form-item label="开始时间">
          <el-date-picker
              v-model="form.begin"
              type="datetime"
              placeholder="开始时间"
              style="width: 100%">
          </el-date-picker>
        </el-form-item>
        <el-form-item label="结束时间">
          <el-date-picker
              v-model="form.end"
              type="datetime"
              placeholder="结束时间"
              style="width: 100%">
          </el-date-picker>
        </el-form-item>
      </el-space>
      <el-form-item label="密码">
        <el-input
            placeholder="请输入密码"
            v-model="form.password">
        </el-input>
      </el-form-item>
      <el-form-item label="描述">
        <el-input
            placeholder="请输入比赛描述"
            type="textarea"
            :autosize="{ minRows: 10}"
            v-model="form.description">
        </el-input>
      </el-form-item>
    </el-form>
    <span style="float: right">
      <el-button @click="vision=false">取消</el-button>
      <el-button @click="create()" type="primary">添加</el-button>
    </span>
  </el-drawer>
  <!--  窗口，修改操作-->
  <el-drawer v-model="editVision" :center="true" title= "添加比赛" size="50%" :with-header="true">
    <el-form :model="editForm" label-width="auto">
      <el-form-item label="比赛名称">
        <el-input v-model="editForm.title" placeholder="请输入名称"></el-input>
      </el-form-item>
      <el-space>
        <el-form-item label="开始时间">
          <el-date-picker
              v-model="editForm.begin"
              type="datetime"
              placeholder="开始时间"
              style="width: 100%">
          </el-date-picker>
        </el-form-item>
        <el-form-item label="结束时间">
          <el-date-picker
              v-model="editForm.end"
              type="datetime"
              placeholder="结束时间"
              style="width: 100%">
          </el-date-picker>
        </el-form-item>
      </el-space>
      <el-form-item label="密码">
        <el-input
            placeholder="请输入密码"
            v-model="editForm.password">
        </el-input>
      </el-form-item>
      <el-form-item label="描述">
        <el-input
            placeholder="请输入比赛描述"
            type="textarea"
            :autosize="{ minRows: 10}"
            v-model="editForm.description">
        </el-input>
      </el-form-item>
    </el-form>
    <span style="float: right">
      <el-button @click="vision=false">取消</el-button>
      <el-button @click="create()" type="primary">添加</el-button>
    </span>
  </el-drawer>

</template>

<style scoped>

</style>

<script setup>
import {Search} from "@element-plus/icons-vue";
import {onMounted, reactive, ref} from 'vue'
import axios from "axios";
import {ElMessage, ElNotification} from "element-plus";
//组件的隐藏显示
const vision = ref(false)
const editVision = ref(false)
//返回值，查找操作
const total = defineModel('total', {default: 0})
const newList = ref([]);
const form = reactive({
  importance: false,//是否置顶
  title: '',//标题
  content: '',//内容
})
const editForm = ref({
  id: '',
  importance: false,//是否置顶
  title: '',//标题
  content: '',//内容
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
  getNewList()
})

function getNewById(id) {
  axios.get('/api/new/'+id, {
    headers: {
      'Authorization': "Bearer " + localStorage.getItem('token'),
    }
  }).then(res => {
    editForm.value = res.data;
  }).catch(err => {
    ElMessage.error(err.message);
  })
}

//增加操作HTTP
function create() {
  axios.post("/api/new", form, {
    headers: {
      'Authorization': "Bearer " + localStorage.getItem('token'),
      'Content-Type': "application/json"
    }
  }).then(() => {
    getNewList()
    vision.value = false
    form.title = ''
    form.content = ''
    form.importance = false
    ElMessage.success("添加成功")
  }).catch(err => {
    ElMessage.error(err.response.data.message);
  })
}

//删除操作HTTP
function remove(id){
  axios.delete('/api/new/'+id, {
    headers: {
      'Authorization': "Bearer " + localStorage.getItem('token'),
    }
  }).then(() => {
    getNewList()
    ElMessage.success("删除成功")
  }).catch(err => {
    ElMessage.error(err.response.data.message);
  })
}

//修改操作HTTP
function update(id) {
  axios.put("/api/new/"+id, editForm.value, {
    headers: {
      'Authorization': "Bearer " + localStorage.getItem('token'),
      'Content-Type': "application/json"
    }
  }).then(() => {
    getNewList()
    editVision.value = false
    form.title = ''
    form.content = ''
    form.importance = false
    ElMessage.success("修改成功")
  }).catch(err => {
    console.log(err)
    // ElMessage.error(err.response.data.message);
  })
}

//查找操作HTTP
function getNewList() {
  axios.defaults.headers.common['Authorization'] = "Bearer " + localStorage.getItem('token');
  axios.get('/api/new' + '?page=' + readForm.page + '&pageSize=' + readForm.pageSize
  ).then(res => {
    newList.value = res.data;
    if (res.data.length === 0) {
      ElMessage.info('没有更多新闻了');
    }
  }).catch(err => {
    ElMessage.error(err.response.data.message);
  })
}
</script>

<template>
  <!--  搜索框-->
  <el-space style="float: right;margin-bottom: 20px">
    <el-button @click="vision=true;">添加</el-button>
    <el-input @input="getNewList" v-model="readForm.value" placeholder="输入数据搜索"
              style="width: 450px;height: auto;margin:auto 50px auto 50px">
      <template #prepend>
        <el-select @change="getNewList()" v-model="readForm.type" placeholder="搜索类型" style="width: 100px">
          <el-option label="新闻编号" value="ID"/>
          <el-option label="标题" value="title"/>
          <el-option label="作者" value="author"/>
          <el-option label="内容" value="content"/>
          <el-option label="更新时间" value="updateTime"/>
        </el-select>
      </template>
      <template #append>
        <el-button @click="getNewList()" :icon="Search"></el-button>
      </template>
    </el-input>
  </el-space>
  <!--  显示的表格-->
  <el-table max-height="80%" v-model:data="newList" border style="width: 100%;margin-top:20px">
    <el-table-column prop="id" label="编号" width="80"/>
    <el-table-column prop="title" label="标题"/>
    <el-table-column prop="author" label="作者"/>
    <el-table-column prop="time" label="更新时间"/>
    <el-table-column label="操作">
      <template #default="item">
        <el-button @click="remove(item.row.id)" link type="primary">删除</el-button>
        <el-button @click="getNewById(item.row.id);editVision=true" link type="primary">编辑</el-button>
        <el-button @click="this.$router.push('/new/' + item.row.id)" link type="primary">详情</el-button>
      </template>
    </el-table-column>
  </el-table>
  <!--  分页组件-->
  <div style="position: absolute;bottom: 15px ">
    <el-pagination
        background
        @current-change="getNewList()"
        @size-change="getNewList()"
        :page-sizes="[8, 12, 50, 100]"
        v-model:current-page="readForm.page"
        v-model:page-size="readForm.pageSize"
        layout="sizes,prev, pager, next,jumper"
        :total="total"/>
  </div>
<!--  窗口，添加操作-->
  <el-drawer v-model="vision" :center="true" title= "添加新闻" size="50%" :with-header="true">
    <el-form :model="form" label-width="auto">
      <el-form-item label="新闻标题">
        <el-input v-model="form.title" placeholder="请输入标题"></el-input>
      </el-form-item>
      <el-form-item label="是否置顶">
        <el-radio-group v-model="form.importance">
          <el-radio size="large" :value=true>是</el-radio>
          <el-radio size="large" :value=false>否</el-radio>
        </el-radio-group>
      </el-form-item>
      <el-form-item label="新闻内容">
        <el-input
            type="textarea"
            :autosize="{ minRows: 10}"
            placeholder="请输入内容"
            v-model="form.content">
        </el-input>
      </el-form-item>
    </el-form>
    <span style="float: right">
      <el-button @click="vision=false">取消</el-button>
      <el-button @click="create()" type="primary">添加</el-button>
    </span>
  </el-drawer>
  <!--  窗口，修改操作-->
  <el-drawer v-model="editVision" :center="true" title= "修改新闻" size="50%" :with-header="true">
    <el-form :model="editForm" label-width="auto">
      <el-form-item label="新闻标题">
        <el-input v-model="editForm.title" placeholder="请输入标题"></el-input>
      </el-form-item>
      <el-form-item label="是否置顶">
        <el-radio-group v-model="editForm.importance">
          <el-radio size="large" :value=true>是</el-radio>
          <el-radio size="large" :value=false>否</el-radio>
        </el-radio-group>
      </el-form-item>
      <el-form-item label="新闻内容">
        <el-input
            type="textarea"
            :autosize="{ minRows: 10}"
            placeholder="请输入内容"
            v-model="editForm.content">
        </el-input>
      </el-form-item>
    </el-form>
      <span style="float: right">
        <el-button @click="editVision=false">取消</el-button>
        <el-button @click="update(editForm.id)" type="primary">修改</el-button>
      </span>
  </el-drawer>

</template>

<style scoped>

</style>

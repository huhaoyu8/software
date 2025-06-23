<script setup>
import {onMounted, ref} from "vue";
import axios from "axios";
import {ElMessage} from "element-plus";
import {useRoute} from "vue-router";
const route = useRoute();
const showProblemList = ref(false);
const contestDetail = ref({
  id: '',
  title: '',
  begin: '',
  end: '',
  description: '',
});
const problemList = ref([]);
function getContestByID() {
  axios.defaults.headers.common['Authorization'] = "Bearer " + localStorage.getItem('token');
  axios.get('/api'+route.path,
  ).then(res => {
    contestDetail.value = res.data;
  }).catch(err => {
    ElMessage.error(err.response.data.message);
  })
}
function getProblemList() {
  showProblemList.value = true;
  axios.defaults.headers.common['Authorization'] = "Bearer " + localStorage.getItem('token');
  axios.get('/api'+route.path+'/problems',
  ).then(res => {
    problemList.value = res.data;
  }).catch(err => {
    ElMessage.error(err.response.data.message);
  })
}
onMounted(() => {
  getContestByID();
})
</script>

<template>
  <div style="width: 100%;height: 100%;padding: 20px">
    <el-progress
        :percentage="50"
        :stroke-width="15"
        status="success"
        striped
        striped-flow
        :duration="20"
        style="margin-bottom: 20px"
    />
  <el-card v-show="!showProblemList" shadow="never" style="width: 70%;float:left;height: 50%">
    <div style="font-size: x-large">{{contestDetail.title}}</div>
    <div style="font-size: medium">比赛描述：{{contestDetail.description}}</div>
  </el-card>
  <el-table border v-show="showProblemList" :data="problemList" style="float: left;width: 70%;height: 50%">
    <el-table-column label="题目编号" prop="id" width="100px"/>
    <el-table-column label="题目名称" prop="title">
      <template #default="item">
        <el-button @click="this.$router.push(route.path+'/problem/'+item.row.id)" style="font-size: 20px" type="primary" text link>{{item.row.title}}</el-button>
      </template>
    </el-table-column>
    <el-table-column label="提交数" prop="submitCount"/>
    <el-table-column label="通过数" prop="acceptedCount"/>
    <el-table-column label="难度" prop="difficulty"/>
  </el-table>
  <el-card shadow="never" style="width: 20%;float: right;">
    <el-space direction="vertical">
      <el-button @click="showProblemList=false" text link type="primary" style="font-size: large" >比赛概要</el-button>
      <el-button @click="getProblemList()" text link type="primary" style="font-size: large" >题目列表</el-button>
    </el-space>
  </el-card>
  <el-card shadow="never" style="float: right;width: 20%;margin-top:20px">
    <ul style="list-style: none;padding: 5px;display: flex;gap:12px;flex-direction: column;">
      <li style="font-size: medium">比赛名称：{{contestDetail.title}}</li>
      <li style="font-size: medium">开始时间：{{contestDetail.begin}}</li>
      <li style="font-size: medium">结束时间：{{contestDetail.end}}</li>
    </ul>
  </el-card>
</div>
</template>

<style scoped>

</style>
<template>
  <el-space direction="vertical" style="margin-left: 8%;float: left">
    <el-card shadow="none" style="width: 800px">
      <template #header>
        <el-space>
          <el-icon><info-filled/></el-icon>
          走马灯
        </el-space>
      </template>
      <el-carousel height="450px" motion-blur>
        <el-carousel-item v-for="item in homeData.url">
          <img :src="item.url" style="width: 100%;height: 100%;object-fit: cover;"/>
        </el-carousel-item>
      </el-carousel>
    </el-card>
    <el-card shadow="none" style="width: 800px;">
      <template #header>
        <el-space>
          <el-icon><info-filled/></el-icon>
          公告
        </el-space>
      </template>
      <el-table :data="homeData.newList">
        <el-table-column label="标题">
          <template #default="item">
            <el-button  style="font-size: 20px" type="primary" text link
                        @click="this.$router.push('/new/' + item.row.id)"
            >{{item.row.title}}</el-button>
          </template>
        </el-table-column>
        <el-table-column label="时间" prop="time"/>
      </el-table>
    </el-card>
  </el-space>

  <el-space direction="vertical" style="margin-right: 5%;float: right">
    <el-card shadow="none" style="width: 350px;float:right;">
      <template #header>
        <el-space>
          <el-icon><Search/></el-icon>
          查找
        </el-space>
      </template>
      <el-input placeholder="请输入题目名称" style="width: 200px;height: 32px;margin-right: 50px">
        <template #append>
          <el-button :icon="Search" />
        </template>
      </el-input>
    </el-card>
    <el-card shadow="none" style="width:350px">
      <template #header>
        <el-space>
          <el-icon><info-filled/></el-icon>
          排行榜
        </el-space>
      </template>
      <el-table :data="homeData.rankList">
        <el-table-column label="姓名" prop="username"/>
        <el-table-column label="排名" prop="top"/>
      </el-table>
    </el-card>
  </el-space>
</template>

<script setup>
import {onMounted, ref} from 'vue';
import axios from "axios";
import {ElMessage} from "element-plus";
import {InfoFilled, Search} from "@element-plus/icons-vue";
const homeData = ref({
  url: [],
  rankList: [],
  newList: []
});
const page=ref(1);
const pageSize = ref(10);

onMounted(()=>{
      getHome();
});
function getHome(){
  // 携带token
  axios.defaults.headers.common['Authorization'] = "Bearer " + localStorage.getItem('token');
  axios.get('/api/' + '?page=' + page.value + '&pageSize=' + pageSize.value
  ).then(res => {
    console.log(res);
    homeData.value = res.data;
  }).catch(err => {
    ElMessage.error(err.message);
  })
}

</script>

<style scoped>
:deep(.el-table .el-table__header th) {
  font-size: 16px; /* 设置标题字体大小 */
  font-weight: bolder;
  color: black;
}
:deep(.el-card__header) {
  font-size: 18px; /* 设置标题字体大小 */
  font-weight: bolder;
  color: black;
  padding: 5px;
  background:rgb(243, 244, 245)
}
:deep(.el-card__body) {
  font-weight: bolder;
  color: black;
  padding: 5px;
}
:deep(.el-table) {
  font-size: 20px; /* 设置所需的字体大小 */
}
</style>

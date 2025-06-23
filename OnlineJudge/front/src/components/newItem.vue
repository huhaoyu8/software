<script setup>

import axios from "axios";
import {ElMessage} from "element-plus";
import {onMounted, ref} from "vue";
import {useRoute} from "vue-router";
const newInfo=ref({
  title: '',
  time: '',
  author:'',
  content: '',
});
const route = useRoute();
function getNewById() {
  axios.get('/api'+route.path, {
    headers: {
      'Authorization': "Bearer " + localStorage.getItem('token'),
    }
  }).then(res => {
    newInfo.value = res.data;
  }).catch(err => {
    ElMessage.error(err.message);
  })
}
onMounted(() => {
  getNewById();
})
</script>

<template>
  <div style="font-size: xxx-large;text-align: center;">{{newInfo.title}}</div>
  <div>
    <span>发表时间：{{newInfo.time}}</span>
    <span style="margin-left:100px ">作者：{{newInfo.author}}</span>
  </div>
  <div style="font-size: x-large">{{newInfo.content}}</div>
</template>

<style scoped>

</style>
<script setup>
import axios from "axios";
import {ElMessage} from "element-plus";
import {onMounted, ref} from "vue";
const info=ref({
  cpu: 0,
  mem: 0,
  disk: 0,
  recv: 0,
  send: 0,
  load: 0,
  host_info:"",
  cpu_info:"",
})
const colors = [
  { color: '#90EE90', percentage: 20 },
  { color: '#3ee434', percentage: 30 },
  { color: '#059807', percentage: 60 },
  { color: '#ff8170', percentage: 80 },
  { color: '#FF0000', percentage: 100 },
]
function getDashboard(){
  axios.get('/api/admin', {
    headers: {
      'Authorization': "Bearer " + localStorage.getItem('token'),
    }
  }).then(res => {
    console.log(res.data);
    info.value = res.data;
  }).catch(err => {
    ElMessage.error(err.response.data);
  })
}
onMounted(() => {
  getDashboard();
})
setInterval(() => {
  getDashboard();
}, 3000);
</script>

<template>
  <el-space size="large" style="width: 100%;display: flex;justify-content: center;gap:90px">
    <el-progress :format="(percentage) => ( 'CPU ' + `${info.cpu}%`)"
                 type="dashboard" :percentage="info.cpu" :color="colors" />
    <el-progress :format="(percentage) => ( '内存 ' + `${info.mem}%`)"
                 type="dashboard" :percentage="info.mem" :color="colors" />
    <el-progress :format="(percentage) => ( '硬盘 ' + `${info.disk}%`)"
                 type="dashboard" :percentage="info.disk" :color="colors" />
    <el-progress :format="(percentage) => ( '负载 ' + `${info.load}%`)"
                 type="dashboard" :percentage="info.load" :color="colors" />
  </el-space>
<pre style="font-size: large">

{{info.host_info}}

{{info.cpu_info}}
  </pre>
</template>

<style scoped>

</style>

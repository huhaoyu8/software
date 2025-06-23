<script setup>
import {ArrowDown, Search} from "@element-plus/icons-vue";

const tableData = [
  {
    date: '2021-09-01',
    name: '1001',
    status: 'Accepted',
    problem: 'A+B',
    duration: '1s',
    memory: '256MB',
    language: 'C++',
    user: 'user1'
  },
  {
    date: '2021-09-01',
    name: '1002',
    status: 'Wrong Answer',
    problem: 'A+B',
    duration: '1s',
    memory: '256MB',
    language: 'C++',
    user: 'user2'
  },
]
</script>

<template>
  <span style="display: flex; justify-content: flex-end;">
  <el-text style="font-size: x-large">状态列表</el-text>
  <span style="flex-grow: 1;" />
  <el-dropdown>
    <span class="el-dropdown-link2" style="color: black;margin-top:-10px">状态<el-icon><arrow-down /></el-icon></span>
    <template #dropdown>
      <el-dropdown-menu>
        <el-dropdown-item>答案正确</el-dropdown-item>
        <el-dropdown-item>答案错误</el-dropdown-item>
        <el-dropdown-item>正在判题中</el-dropdown-item>
        <el-dropdown-item>等待判题</el-dropdown-item>
        <el-dropdown-item>格式错误</el-dropdown-item>
        <el-dropdown-item>运行时错误</el-dropdown-item>
        <el-dropdown-item>编译错误</el-dropdown-item>
        <el-dropdown-item>内存超限</el-dropdown-item>
        <el-dropdown-item>运行超时</el-dropdown-item>
        <el-dropdown-item>输出超限</el-dropdown-item>
      </el-dropdown-menu>
    </template>
  </el-dropdown>
  <el-switch
    size="large"
    style="height: 32px;margin: 0 50px"
    inline-prompt
    active-text="我的"
    inactive-text="全部"/>
  <el-input placeholder="搜索作者" style="width: 200px;height: 32px;margin-right: 50px">
      <template #append>
        <el-button :icon="Search" />
      </template>
  </el-input>
  <el-button>刷新</el-button>
  </span>
<el-table :data="tableData" stripe style="width: 100%">
  <el-table-column prop="date" label="时间" width="180" />
  <el-table-column prop="name" label="ID" width="180" />
  <el-table-column prop="status" label="状态" />
  <el-table-column prop="problem" label="问题" />
  <el-table-column prop="duration" label="时长" />
  <el-table-column prop="memory" label="内存" />
  <el-table-column prop="language" label="语言" />
  <el-table-column prop="user" label="用户" />
</el-table>
</template>

<style scoped>
.el-dropdown-link2  {
  cursor: pointer;
  color: var(--el-color-primary);
  display: flex;
  align-items: center;
}
.el-dropdown-link2{
  font-size: larger;
}
.el-dropdown-link2:focus {
  outline: none;
}
</style>

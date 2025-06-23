<script setup>
import {ArrowDown, Search} from "@element-plus/icons-vue";

const tableData = [
  {
    number:1,
    name: 'user1',
    Rating:12,
    status: 'AC',
  }
]
</script>

<template>
  <span style="display: flex; justify-content: flex-end;">
  <el-text style="font-size: x-large">排行榜</el-text>
  <span style="flex-grow: 1;" />
  <el-dropdown>
    <span class="el-dropdown-link2" style="color: black;margin-top:-10px">排名依据<el-icon><arrow-down /></el-icon></span>
    <template #dropdown>
      <el-dropdown-menu>
        <el-dropdown-item>CUGOJ</el-dropdown-item>
        <el-dropdown-item>Codeforces</el-dropdown-item>
      </el-dropdown-menu>
    </template>
  </el-dropdown>
  <el-input placeholder="用户名搜索" style="width: 200px;height: 32px;margin-right: 50px">
      <template #append>
        <el-button :icon="Search" />
      </template>
  </el-input>
  <el-button>刷新</el-button>
</span>

<el-table :data="tableData" stripe style="width: 100%">
  <el-table-column prop="number" label="#" />
  <el-table-column prop="name" label="用户"  />
  <el-table-column prop="Rating" label="Rating"/>
  <el-table-column prop="status" label="AC" />
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

<template>
  <el-container>
    <el-aside style="max-width: 150px">
      <el-menu style="user-select: none" router>
        <el-text style="font-size: x-large;">管理员</el-text>
        <el-menu-item class="leftItem" index="/admin"><el-icon><Menu /></el-icon>概览</el-menu-item>
        <el-menu-item class="leftItem" index="/admin/new"><el-icon><List /></el-icon>新闻</el-menu-item>
<!--        <el-menu-item class="leftItem" index="/admin/user"><el-icon><Avatar /></el-icon>用户</el-menu-item>-->
        <el-menu-item class="leftItem" index="/admin/problem"><el-icon><Document /></el-icon>题目</el-menu-item>
        <el-menu-item class="leftItem" index="/admin/contest"><el-icon><Medal /></el-icon>竞赛</el-menu-item>
<!--        <el-menu-item class="leftItem" index="/admin/judgeServer"><el-icon><Cpu /></el-icon>判题机</el-menu-item>-->
<!--        <el-menu-item class="leftItem" index="/admin/config"><el-icon><Tools /></el-icon>系统配置</el-menu-item>-->
<!--        <div style=""></div>-->
<!--        自动填充满-->
        <div style="height: 300px"></div>
      </el-menu>
    </el-aside>
    <el-main>
    <router-view></router-view>
    </el-main>
  </el-container>
</template>
<script lang="ts" setup>
import {Avatar, Cpu, Document, List, Medal, Menu, Tools} from "@element-plus/icons-vue";
</script>
<style>
.leftItem{
  font-size: 18px;
}
</style>

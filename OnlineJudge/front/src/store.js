import {ref, watch} from 'vue';

export const username = ref(localStorage.getItem('username'));
export const tokenExist = ref('token' in localStorage);
watch(username, (newValue) => {
	username.value = newValue;
	localStorage.setItem('username', newValue);
});
watch(tokenExist, (newValue) => {
	tokenExist.value = newValue;
	localStorage.setItem('tokenExist', newValue);
})
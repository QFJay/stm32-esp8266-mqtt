<template>
	<view class="wrap">
		<view class="device-area">
			<view class="device-card">
				<view class="">
					<view class="device-name">湿度</view>
					<image class="device-logo" src="/static/humidity.png" mode=""></image>
				</view>
				<view class="device-data">{{humidity}} %RH</view>
			</view>
			<view class="device-card">
				<view class="">
					<view class="device-name">温度</view>
					<image class="device-logo" src="/static/temperature.png" mode=""></image>
				</view>
				<view class="device-data">{{temperature}} ℃</view>
			</view>
			<view class="device-card">
				<view class="">
					<view class="device-name">光照强度</view>
					<image class="device-logo" src="/static/light.png" mode=""></image>
				</view>
				<view class="device-data">{{light}} %</view>
			</view>
			<view class="device-card">
				<view class="">
					<view class="device-name">LED</view>
					<image class="device-logo" src="/static/LED.png" mode=""></image>
				</view>
				<switch :checked="LED" @change="onLEDSwitch" color="#1296db" />
			</view>
		</view>
	</view>
</template>

<script>
	import createCommonToken from '@/key.js'
	export default {
		data() {
			return {
				// 设备数据
				humidity: '--',
				temperature: '--',
				light: '--',
				LED: false,

				// 轮询定时器
				pollingTimer: null,

				// API 配置
				apiConfig: {
					productId: 'Roh7b244ZQ',
					deviceName: 'device1',
					token: ''
				}
			}
		},
		onLoad() {
			// 生成 token
			const params = {
				access_key: 'LTm7Q03VJVF9w/uf0e+zWRUxZR+1FeEtx8E9a3VPyw8=',
				version: '2022-05-01',
				productid: this.apiConfig.productId
			};
			this.apiConfig.token = createCommonToken(params)
		},
		onShow() {
			// 页面显示时：立即获取一次，然后开始轮询
			this.fetchDeviceData();
			this.startPolling()
		},
		onHide() {
			// 页面隐藏时：停止轮询（节省资源）
			this.stopPolling()
		},
		onUnload() {
			// 页面卸载时：彻底清除定时器
			this.stopPolling()
		},
		methods: {
			// 开始轮询（每2秒获取一次）
			startPolling() {
				this.stopPolling();
				this.pollingTimer = setInterval(() => {
					this.fetchDeviceData()
				}, 2000)
			},

			// 停止轮询
			stopPolling() {
				if (this.pollingTimer) {
					clearInterval(this.pollingTimer);
					this.pollingTimer = null
				}
			},

			// 获取设备最新数据
			fetchDeviceData() {
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/query-device-property',
					method: 'GET',
					data: {
						product_id: this.apiConfig.productId,
						device_name: this.apiConfig.deviceName
					},
					header: {
						'authorization': this.apiConfig.token
					},
					success: (res) => {
						if (res.data && res.data.data) {
							console.log(res.data);

							const getValue = (id) => {
								const item = res.data.data.find(item => item.identifier === id);
								return item ? item.value : '--'
							};

							this.humidity = getValue('humidity');
							this.temperature = getValue('temperature');
							this.light = getValue('light');
							this.LED = (getValue('LED') === 'true')
						}
					},
					fail: (err) => {
						console.error('获取数据失败', err)
					}
				})
			},

			// 控制 LED 开关
			onLEDSwitch(event) {
				let LED_value = event.detail.value;

				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property',
					method: 'POST',
					data: {
						product_id: this.apiConfig.productId,
						device_name: this.apiConfig.deviceName,
						params: {
							LED: LED_value
						}
					},
					header: {
						'authorization': this.apiConfig.token
					},
					success: () => {
						console.log('LED ' + (LED_value ? '开启' : '关闭'))
					},
					fail: (err) => {
						console.error('控制 LED 失败', err);
						uni.showToast({
							title: '控制失败，请检查网络',
							icon: 'none'
						})
					}
				})
			}
		}
	}
</script>

<style>
	.wrap {
		padding: 30rpx;
	}

	.device-area {
		display: flex;
		justify-content: space-between;
		flex-wrap: wrap;
	}

	.device-card {
		height: 160rpx;
		width: 320rpx;
		border-radius: 30rpx;
		margin-top: 30rpx;
		display: flex;
		justify-content: space-around;
		align-items: center;
		box-shadow: 0 0 15rpx #ccc;
	}

	.device-name {
		font-size: 26rpx;
		text-align: center;
		color: #6d6d6d;
	}

	.device-logo {
		height: 90rpx;
		width: 90rpx;
		margin-top: 10rpx;
	}

	.device-data {
		font-size: 40rpx;
		color: #6d6d6d;
	}
</style>
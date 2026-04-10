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
				humidity: '',
				temperature: '',
				light: '',
				LED: false,
				token: ''
			}
		},
		onLoad() {
			const params = {
				access_key: 'LTm7Q03VJVF9w/uf0e+zWRUxZR+1FeEtx8E9a3VPyw8=',
				version: '2022-05-01',
				productid: 'Roh7b244ZQ'
			}

			this.token = createCommonToken(params);
		},
		onShow() {
			// this.fetchDeviceData();
			setInterval(()=>{
				this.fetchDeviceData();
			}, 2000)
		},
		methods: {
			fetchDeviceData() {
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/query-device-property',
					method: 'GET',
					data: {
						product_id: 'Roh7b244ZQ',
						device_name: 'device1'
					},
					header: {
						'authorization': this.token
					},
					success: (res) => {
						console.log(res.data);
						this.humidity = res.data.data[1].value;
						this.temperature = res.data.data[3].value;
						this.light = res.data.data[2].value;
						this.LED = (res.data.data[0].value === "true");
					}
				});
			},
			onLEDSwitch(event) {
				let LED_value = event.detail.value;

				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property',
					method: 'POST',
					data: {
						product_id: 'Roh7b244ZQ',
						device_name: 'device1',
						params: {
							"LED": LED_value
						}
					},
					header: {
						'authorization': this.token
					},
					success: () => {
						console.log('LED ' + (LED_value ? 'on' : 'off'));
					}
				});
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
// const crypto = require('crypto')
import CryptoJS from 'crypto-js'

function createCommonToken(params) {
	// const access_key = Buffer.from( params.access_key, "base64")
	const access_key = CryptoJS.enc.Base64.parse(params.access_key)

	const version = params.version
	let res = 'products' + '/' + params.productid
	const et = Math.ceil((Date.now() + 365 * 24 * 3600 * 1000) / 1000)
	const method = 'sha1'

	const key = et + "\n" + method + "\n" + res + "\n" + version

	// let sign = crypto.createHmac('sha1', access_key).update(key).digest().toString('base64')
	const hmac = CryptoJS.HmacSHA1(key, access_key)
	let sign = hmac.toString(CryptoJS.enc.Base64)

	res = encodeURIComponent(res)
	sign = encodeURIComponent(sign)
	const token = `version=${version}&res=${res}&et=${et}&method=${method}&sign=${sign}`

	return token
}

// module.exports = {
//   createCommonToken
// };

export default createCommonToken
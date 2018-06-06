"use strict"

var fs              = require("fs")
var ndarray         = require("ndarray")
var savePixels      = require("save-pixels")
var naData          = require("./cpp-naive");
var roData          = require("./cpp-robust");

var NX = 512;
var NY = 512;

function naiveLeftRight(a, b, c) {
	var abx = c[0] - a[0]
	var aby = c[1] - a[1]
	var acx = b[0] - a[0]
	var acy = b[1] - a[1]
	return abx * acy - aby * acx
}

function plotPredicateUD(data) {
	var img = ndarray(new Uint8Array(NX * NY * 3), [NX, NY, 3]);
	var idx = 0;
	for(var i = 0; i < NX; ++i) {
		for(var j = 0; j < NY; ++j) {
			var o = data[idx];
			if(o < 0) {
				img.set(i, j, 2, 255)
			} else if(o > 0) {
				img.set(i, j, 0, 255)
			} else {
				img.set(i, j, 1, 255)
			}
			idx += 1
		}
	}
	//imshow(img)
	return savePixels(img, "png")
}

console.log("naive predicate")
plotPredicateUD(naData).pipe(fs.createWriteStream(__dirname + "/naive-lr.png"));

console.log("robust predicate");
plotPredicateUD(roData).pipe(fs.createWriteStream(__dirname + "/robust-lr.png"));

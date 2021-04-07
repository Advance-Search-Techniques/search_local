const addon = require('bindings')('search')

const vector = new addon.Vector(10,20,15)
console.log(vector.x)

const vector2 = new addon.Vector(11,23,45)

console.log(vector.add(vector2))
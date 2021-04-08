const {TextGps} = require("bindings")('search')

const database = new TextGps()
console.log(database.open("C:\\Users\\qin\\Desktop\\123.db"))
database.getResult(123)
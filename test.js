// hello.js
const amisxml = require('./build/Release/node-amisxml');
var xpath = require('xpath')
  , dom = require('xmldom').DOMParser

const params = {
  "PerId": "4F47ACC8F02B4F7BA9FBF6E7050E810C", // Süß Ute on SchulungsDB
};
xml = amisxml.get_service_object("AmisXML.CO_Anwender2SammelNrAsJWS001", {"OhneDTD": "true"});
console.log("xml", xml);

const amisxml = require('../build/Release/node-amisxml');
const xpath = require('xpath');
const xmldom = require('xmldom');

describe("node-ammisxml proxy_for_url", () => {
  it("current proxy", () => {
    let p = amisxml.proxy_for_url("http://amis20-qs.azd.io");
    expect(p).toEqual("proxy.vertrieb.allianz:80");
  });
  it("pac_url", () => {
    let p = amisxml.pac_url();
    expect(p).toBeDefined();
  });
});

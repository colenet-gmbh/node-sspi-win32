const amisxml = require('../build/Release/node-amisxml');
const xpath = require('xpath');
const xmldom = require('xmldom');

describe("node-ammisxml proxy_for_url", () => {
  it("machine name", () => {
    let p = amisxml.machine_name();
    expect(p).toEqual("RALF35AB");
  });
  it("pac_url", () => {
    let p = amisxml.pac_url();
    expect(p).toBeDefined();
  });
});

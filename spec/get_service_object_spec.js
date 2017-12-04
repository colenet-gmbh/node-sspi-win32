const amisxml = require('../build/Release/node-amisxml')
const xpath = require('xpath');
const xmldom = require('xmldom');

describe("node-ammisxml", () => {
  it("offers get_service_object with parameters", () => {
    const params = {
      "PerId" :"8CAB6F39341211E480BB028037EC0200",
      "OhneDTD": "TRUE",
      "IgnoreEmpty": "FALSE"
    };
    let xml = amisxml.get_service_object("AmisXML.CO_PerBank002", params );
    expect(xml).toContain("PBA_BLZ")
  });

  it("throws if no COCLASS is present", () => {
    var no_args = function() {
      amisxml.get_service_object();
    };
    expect(no_args).toThrow();
  });

  it("throws if no req is present", () => {
    var only_coclass = function() {
      amisxml.get_service_object("CO_ActAnwender001");
    };
    expect(only_coclass).toThrow();
  });

  it("throws parameter is not supported by service object", () => {
    var invalid_param = function() {
      amisxml.get_service_object("CO_ActAnwender001", {"KeinGültigerparam": "asdfadf"});
    };
    expect(invalid_param).toThrow();
  });

  it("handles character encoding corret", () => {
    const params = {
      "PerId": "4F47ACC8F02B4F7BA9FBF6E7050E810C", // Süß Ute on SchulungsDB
    };
    xml = amisxml.get_service_object("AmisXML.CO_Person001A", params);
    doc = new xmldom.DOMParser().parseFromString(xml);
    name = xpath.select1("//PER_NAME/text()", doc);
    expect(name.data).toEqual("Süß");
  });
});

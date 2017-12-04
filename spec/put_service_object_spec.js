const amisxml = require('../build/Release/node-amisxml');
const xpath = require('xpath');
const xmldom = require('xmldom');

describe("node-ammisxml put_service_object", () => {
  it("can update an XML Documemt previously read using SchulungsDB", () => {
    // SchulungsDB: Luisa Bayer (Interessent) -> Eine Bankverbindung
    const query_params = {
      "PerId" :"23FC669513F111E4A074028037EC0200",
      "OhneDTD": "TRUE",
      "IgnoreEmpty": "False"
    };
    const insert_params = {
      "OhneDTD": "TRUE",
      "IgnoreEmpty": "False"
    };

    let query_xml = amisxml.get_service_object("AmisXML.CO_AllgVorgang002", query_params );
    let query_doc = new xmldom.DOMParser().parseFromString(query_xml);
    let count = xpath.select1("count(//AllgVorgang002)", query_doc);
    let xml = amisxml.get_service_object("AmisXML.CO_AllgVorgang002", insert_params );
    let doc = new xmldom.DOMParser().parseFromString(xml);

    let vor_id = xpath.select1("//AllgVorgang_Vor_Id", doc);
    let text = doc.createTextNode("ClientsideId");
    vor_id.appendChild(text);

    let per_id = xpath.select1("//AllgVorgang_Per_Id", doc);
    text = doc.createTextNode("23FC669513F111E4A074028037EC0200");
    per_id.appendChild(text);

    let art = xpath.select1("//AllgVorgang_Art", doc);
    text = doc.createTextNode("22");
    art.appendChild(text);

    let inhalt = xpath.select1("//AllgVorgang_Inhalt", doc);
    text = doc.createTextNode("Dies ist ein Test ÄÖÜ€");
    inhalt.appendChild(text);

    let new_xml = new xmldom.XMLSerializer().serializeToString(doc)
    let id_mapping = amisxml.put_service_object("AmisXML.CO_AllgVorgang002", new_xml);
    idm_doc = new xmldom.DOMParser().parseFromString(id_mapping);

    let amisid = xpath.select1("//AMISID/text()", idm_doc).data;
    expect(xpath.select1("//FREMDID/text()", idm_doc).data).toEqual("ClientsideId");
    expect(amisid).toBeDefined();

    query_xml = amisxml.get_service_object("AmisXML.CO_AllgVorgang002", query_params );
    query_doc = new xmldom.DOMParser().parseFromString(query_xml);
    let new_count = xpath.select1("count(//AllgVorgang002)", query_doc);
    expect(new_count).toEqual(count+1);

    const read_params = {
      "VorId": amisid
    };
    query_xml = amisxml.get_service_object("AmisXML.CO_AllgVorgang002", read_params );
    query_doc = new xmldom.DOMParser().parseFromString(query_xml);
    expect(xpath.select1("//AllgVorgang_Inhalt/text()", query_doc).data).toEqual("Dies ist ein Test ÄÖÜ€");
  });

  it("throws if params dont match type", () => {
    var invalid_param = function() {
      amisxml.put_service_object("CO_ActAnwender001", {"KeinGültigerparam": "asdfadf"});
    };
    expect(invalid_param).toThrow();

    invalid_param = function() {
      amisxml.put_service_object("CO_ActAnwender001", "No valid xml");
    };
    expect(invalid_param).toThrow();

    invalid_param = function() {
      amisxml.put_service_object(12, "No valid xml");
    };
    expect(invalid_param).toThrow();
  });

});

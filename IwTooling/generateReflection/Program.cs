using System;
using System.IO;
using System.Collections.Generic;
using Antlr4.StringTemplate;
using Newtonsoft.Json;

namespace generateReflection
{
    class targ {
        public string name;
        public string type_name;
    }

    class field
    {
        public bool is_base;
        public bool is_last;
        public int index;
        public string name;
        public string type_name;
    }

    class record {
        public string type_name;
        public List<targ> targs;
        public List<field> fields;
        public List<string> bases;
    }

    class records_json {
        public List<record> records;
    }

    class Program
    {
        static void Main(string[] args)
        {
            string template_string = File.ReadAllText("C:/dev/IwEngine/IwTooling/generateReflection/template.txt");

            records_json records = JsonConvert.DeserializeObject<records_json>(File.ReadAllText("C:/dev/IwEngine/IwTooling/reflector/src/test.json"));

            foreach (record record in records.records)
            {

                if (record.bases != null && record.bases.Count > 0)
                {
                    for (int i = record.bases.Count - 1; i >= 0; i--)
                    {
                        field f = new field();
                        f.name = null;
                        f.type_name = record.bases[i];
                        f.is_base = true;

                        record.fields.Insert(0, f);
                    }
                }

                if (record.fields != null && record.fields.Count > 0)
                {
                    for (int i = 0; i < record.fields.Count; i++)
                    {
                        record.fields[i].index = i;
                        record.fields[i].is_last = i == record.fields.Count - 1;
                    }
                }

                int targ_count  = record.targs  == null ? 0 : record.targs .Count;
                int field_count = record.fields == null ? 0 : record.fields.Count;
                int base_count  = record.bases  == null ? 0 : record.bases .Count;

                string targs = "";

                if (targ_count > 0)
                {
                    for (int i = 0; i < targ_count; i++)
                    {
                        targ arg = record.targs[i];

                        targs += arg.type_name + " " + arg.name; 
                        if (i < targ_count - 1)
                        {
                            targs += ", ";
                        }
                    }
                }

                Template template = new Template(template_string);
                template.Add("type_name",   record.type_name);
                template.Add("has_targs",   targ_count > 0);
                template.Add("targs",       targs);
                template.Add("field_count", field_count);
                template.Add("fields",      record.fields);

                string str = template.Render();
                Console.WriteLine(str);
            }
        }
    }
}

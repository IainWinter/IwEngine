using System;
using System.IO;
using Antlr4.StringTemplate;

namespace generateReflection
{

    class field
    {
        public string name;
        public string type_name;

        public field(string name, string type)
        {
            this.name = name;
            this.type_name = type;
        }
    }
    class Program
    {
        static void Main(string[] args)
        {

            string name = "test_class";
            string[] bases =
            {
                "test_base_class",
                "another_interface"
            };
            field[] fields =
            {
                new field("x", "int"),
                new field("y", "int")
            };

            Template template = new Template(File.ReadAllText("C:/dev/IwEngine/IwTooling/generateReflection/template.txt"));
            template.Add("type_name",   name);
            template.Add("bases",       bases);
            template.Add("field_count", fields.Length);
            template.Add("fields",      fields);

            string str = template.Render();
            Console.WriteLine(str);
        }
    }
}

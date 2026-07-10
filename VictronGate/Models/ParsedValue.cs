namespace VictronGate.Models
{
    public class ParsedValue
    {
        public string Name { get; set; } = string.Empty;  

        public string Value { get; set; } = string.Empty;

        public ParsedValue(string name, string value)
        {
            Name = name;
            Value = value;
        }

        public override string ToString()
        {
            return $"{Name}: {Value}";
        }

        public override bool Equals(object? obj)
        {
            if (obj is ParsedValue other)
            {
                return Name == other.Name && Value == other.Value;
            }
            return false;
        }

        public bool IsEmpty()
        {
            return string.IsNullOrEmpty(Name) && string.IsNullOrEmpty(Value);
        }

        public static ParsedValue Empty => new ParsedValue(string.Empty, string.Empty);

        public override int GetHashCode()
        {
            return ToString().GetHashCode();   
        }
    }
}

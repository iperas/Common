namespace GreisDocParser
{
    /// <summary>
    /// ��� ����������:
    /// �������� ������ ������� Fill (����� ���� - ������������ ������ (� ����������� ������)).
    /// 
    /// ��� �������� � ���������:
    /// Fill = ���� � ������������ ������ ����������� ���������� �� ������� fieldCount = (struct_size - static_fields_size) / (the_other_fields_size)
    /// Dynamic = ������� ����� ��������� ��������� ��������� ����������� ���������.
    /// </summary>
    public enum SizeSpecialValue
    {
        Dynamic = -1,   // Size is computing according unknown special logic
        Fill = -2,      // Size is computing by formula: (struct size) - (other fields size)
    }
}
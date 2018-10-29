namespace Arc.Ddsi.Xp3Pack
{
    internal class Adler32
    {
        private int _a = 1;
        private int _b = 0;

        public int Checksum
        {
            get
            {
                return _b * 65536 + _a;
            }
        }

        private const int Modulus = 65521;

        public void Update(byte[] data, int offset, int length)
        {
            for (int counter = 0; counter < length; ++counter)
            {
                _a = (_a + (data[offset + counter])) % Modulus;
                _b = (_b + _a) % Modulus;
            }
        }
    }
}

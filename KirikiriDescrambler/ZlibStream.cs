using System;
using System.IO;
using System.IO.Compression;

namespace Arc.Ddsi.KirikiriDescrambler
{
    internal class ZlibStream : DeflateStream
    {
        private readonly Adler32 _adler32 = new Adler32();

        public ZlibStream(Stream innerStream)
            : base(innerStream, CompressionMode.Compress, true)
        {
            byte[] header = { 0x78, 0x9C };
            BaseStream.Write(header, 0, 2);
        }

        public override void Write(byte[] array, int offset, int count)
        {
            base.Write(array, offset, count);
            _adler32.Update(array, offset, count);
        }

        protected override void Dispose(bool disposing)
        {
            Stream baseStream = BaseStream;
            base.Dispose(disposing);

            byte[] checksum = BitConverter.GetBytes(_adler32.Checksum);
            for (int i = 3; i >= 0; i--)
            {
                baseStream.WriteByte(checksum[i]);
            }
        }
    }
}

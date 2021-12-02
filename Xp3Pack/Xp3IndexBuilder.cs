using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Arc.Ddsi.Xp3Pack
{
    internal class Xp3IndexBuilder
    {
        private static class ChunkType
        {
            public const uint File = 0x656C6946;
            public const uint Info = 0x6F666E69;
            public const uint Segment = 0x6D676573;
            public const uint Checksum = 0x726C6461;
        }

        private readonly Stream _index;
        private readonly BinaryWriter _writer;
        private readonly Stack<long> _chunkOffsets;

        public Xp3IndexBuilder()
        {
            _index = new MemoryStream();
            _writer = new BinaryWriter(_index);
            _chunkOffsets = new Stack<long>();

            _writer.Write((byte)0);      // Index flag
            _writer.Write(0L);           // Index size
        }

        public void Add(string fileName, long offset, long originalSize, long compressedSize, bool compressed)
        {
            WriteFileChunk(fileName, offset, originalSize, compressedSize, compressed);
        }

        public byte[] Build()
        {
            UpdateIndexSize();

            _index.Seek(0, SeekOrigin.Begin);
            byte[] result = new byte[_index.Length];
            _index.Read(result, 0, result.Length);
            return result;
        }

        private void WriteFileChunk(string fileName, long offset, long originalSize, long compressedSize, bool compressed)
        {
            BeginChunk(ChunkType.File);
            WriteInfoChunk(fileName, originalSize, compressedSize);
            WriteSegmentChunk(offset, originalSize, compressedSize, compressed);
            WriteChecksumChunk();
            EndChunk();
        }

        private void WriteInfoChunk(string fileName, long originalSize, long compressedSize)
        {
            BeginChunk(ChunkType.Info);
            _writer.Write(0);                               // Flags
            _writer.Write(originalSize);                    // Original size
            _writer.Write(compressedSize);                  // Compressed size
            _writer.Write((ushort)fileName.Length);
            _writer.Write(Encoding.Unicode.GetBytes(fileName));
            EndChunk();
        }

        private void WriteSegmentChunk(long offset, long originalSize, long compressedSize, bool compressed)
        {
            BeginChunk(ChunkType.Segment);
            _writer.Write(Convert.ToInt32(compressed));     // Flags (1 = compressed)
            _writer.Write(offset);                          // Offset in archive
            _writer.Write(originalSize);                    // Original size
            _writer.Write(compressedSize);                  // Compressed size
            EndChunk();
        }

        private void WriteChecksumChunk()
        {
            BeginChunk(ChunkType.Checksum);
            _writer.Write(0);           // Checksum
            EndChunk();
        }

        private void BeginChunk(uint type)
        {
            _writer.Write(type);
            _writer.Write(0L);
            _chunkOffsets.Push(_index.Length);
        }

        private void EndChunk()
        {
            long startOffset = _chunkOffsets.Pop();
            long length = _index.Length - startOffset;
            _index.Seek(startOffset - 8, SeekOrigin.Begin);
            _writer.Write(length);
            _index.Seek(0, SeekOrigin.End);
        }

        private void UpdateIndexSize()
        {
            _index.Seek(1, SeekOrigin.Begin);
            _writer.Write(_index.Length - 9);
            _index.Seek(0, SeekOrigin.End);
        }
    }
}

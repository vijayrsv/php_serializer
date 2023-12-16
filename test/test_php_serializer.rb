require 'minitest/autorun'
require 'php_serializer'

describe PhpSerializer do
	before do
		@serializer = PhpSerializer
	end

	describe '#serialize' do
		it 'serializes a string' do
			result = @serializer.serialize('Hello, World!')
			assert_equal 's:13:"Hello, World!";', result
		end

		it 'serializes a number' do
			result = @serializer.serialize(123)
			assert_equal 'i:123;', result
		end

		it 'serializes an array' do
			result = @serializer.serialize([1, 2, 3])
			assert_equal 'a:3:{i:0;i:1;i:1;i:2;i:2;i:3;}', result
		end

		it 'serializes a boolean true' do
			result = @serializer.serialize(true)
			assert_equal 'b:1;', result
		end

		it 'serializes a boolean false' do
			result = @serializer.serialize(false)
			assert_equal 'b:0;', result
		end

		it 'serializes null' do
			result = @serializer.serialize(nil)
			assert_equal 'N;', result
		end

		it 'serializes an empty array' do
			result = @serializer.serialize([])
			assert_equal 'a:0:{}', result
		end

		it 'serializes an array with elements' do
			result = @serializer.serialize(['a', 'b', 'c'])
			assert_equal 'a:3:{i:0;s:1:"a";i:1;s:1:"b";i:2;s:1:"c";}', result
		end

		it 'serializes an empty hash' do
			result = @serializer.serialize({})
			assert_equal 'a:0:{}', result
		end

		it 'serializes a hash with elements' do
			result = @serializer.serialize({'a' => 1, 'b' => 2})
			assert_equal 'a:2:{s:1:"a";i:1;s:1:"b";i:2;}', result
		end

		it 'serializes a nested array' do
			result = @serializer.serialize([['a', 'b'], ['c', 'd']])
			assert_equal 'a:2:{i:0;a:2:{i:0;s:1:"a";i:1;s:1:"b";}i:1;a:2:{i:0;s:1:"c";i:1;s:1:"d";}}', result
		end

		it 'serializes a nested hash' do
			result = @serializer.serialize({'a' => {'b' => 2}, 'c' => {'d' => 4}})
			assert_equal 'a:2:{s:1:"a";a:1:{s:1:"b";i:2;}s:1:"c";a:1:{s:1:"d";i:4;}}', result
		end
	end

	describe '#unserialize' do
		it 'unserializes a string' do
			result = @serializer.unserialize('s:13:"Hello, World!";')
			assert_equal 'Hello, World!', result
		end

		it 'unserializes a number' do
			result = @serializer.unserialize('i:123;')
			assert_equal 123, result
		end

		it 'unserializes a boolean true' do
			result = @serializer.unserialize('b:1;')
			assert_equal true, result
		end

		it 'unserializes a boolean false' do
			result = @serializer.unserialize('b:0;')
			assert_equal false, result
		end

		it 'unserializes null' do
			result = @serializer.unserialize('N;')
			assert_nil result
		end

		it 'unserializes an empty array' do
			result = @serializer.unserialize('a:0:{}')
			assert_equal [], result
		end

		it 'unserializes an array with elements' do
			result = @serializer.unserialize('a:3:{i:0;s:1:"a";i:1;s:1:"b";i:2;s:1:"c";}')
			assert_equal ['a', 'b', 'c'], result
		end

		it 'unserializes an empty hash' do
			result = @serializer.unserialize('a:0:{}')
			assert_equal([], result) # Cannot differentiate between array and hash in PHP
		end

		it 'unserializes a hash with elements' do
			result = @serializer.unserialize('a:2:{s:1:"a";i:1;s:1:"b";i:2;}')
			assert_equal({'a' => 1, 'b' => 2}, result)
		end

		it 'unserializes a nested array' do
			result = @serializer.unserialize('a:2:{i:0;a:2:{i:0;s:1:"a";i:1;s:1:"b";}i:1;a:2:{i:0;s:1:"c";i:1;s:1:"d";}}')
			assert_equal [['a', 'b'], ['c', 'd']], result
		end

		it 'unserializes a nested hash' do
			result = @serializer.unserialize('a:2:{s:1:"a";a:1:{s:1:"b";i:2;}s:1:"c";a:1:{s:1:"d";i:4;}}')
			assert_equal({'a' => {'b' => 2}, 'c' => {'d' => 4}}, result)
		end
	end

	describe '#unserialize_incomplete' do
		it 'raises an error for an incomplete string' do
			assert_raises(TypeError) do
				@serializer.unserialize('s:5:"Hello')
			end
		end

		it 'raises an error for an incomplete number' do
			assert_raises(TypeError) do
				@serializer.unserialize('i:123')
			end
		end

		it 'raises an error for an incomplete boolean' do
			assert_raises(TypeError) do
				@serializer.unserialize('b:1')
			end
		end

		it 'raises an error for an incomplete null' do
			assert_raises(TypeError) do
				@serializer.unserialize('N')
			end
		end

		it 'raises an error for an incomplete empty array' do
			assert_raises(TypeError) do
				@serializer.unserialize('a:0:{')
			end
		end

		it 'raises an error for an incomplete array with elements' do
			assert_raises(TypeError) do
				@serializer.unserialize('a:3:{i:0;s:1:"a";i:1;s:1:"b";i:2;s:1:"c"')
			end
		end

		it 'raises an error for an incomplete empty hash' do
			assert_raises(TypeError) do
				@serializer.unserialize('a:0:{')
			end
		end

		it 'raises an error for an incomplete hash with elements' do
			assert_raises(TypeError) do
				@serializer.unserialize('a:2:{s:1:"a";i:1;s:1:"b";i:2')
			end
		end

		it 'raises an error for an incomplete nested array' do
			assert_raises(TypeError) do
				@serializer.unserialize('a:2:{i:0;a:2:{i:0;s:1:"a";i:1;s:1:"b";}i:1;a:2:{i:0;s:1:"c";i:1;s:1:"d"')
			end
		end

		it 'raises an error for an incomplete nested hash' do
			assert_raises(TypeError) do
				@serializer.unserialize('a:2:{s:1:"a";a:1:{s:1:"b";i:2;}s:1:"c";a:1:{s:1:"d";i:4')
			end
		end
	end 
end

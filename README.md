# PhpSerializer

Native PHP serializer and unserializer for Ruby.  It is heavily
inspired by PHP source code.

Note: Currently only supports PHP primitive data-types.

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'php_serializer'
```

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install php_serializer

## Ruby versions supported

Being a native gem, this library requires MRI (Matz's Ruby
Interpreter).  It requires MRI 1.9.3 or greater.  It is known to build
with these Ruby versions:

* ruby-1.9.3
* ruby-2.0
* ruby-2.1
* ruby-2.2
* ruby-2.3
* ruby-2.4
* ruby-2.7
* ruby-3.1

## Usage

``` ruby
PhpSerializer.serialize('example')
PhpSerializer.unserialize('s:7:"example";')
```
## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/vijayrsv/php_serializer.

## Building from source
### C file
``` bash
re2c -o ext/php_serializer/php_serializer.c ext/php_serializer/php_serializer.re
```

### Gem
``` bash
gem build php_serializer.gemspec
gem install php_serializer-0.2.1.gem
```

### Running tests
``` bash
ruby -Ilib:test test/test_php_serializer.rb
```

## License

The gem is available as open source under the terms of the [MIT License](http://opensource.org/licenses/MIT).

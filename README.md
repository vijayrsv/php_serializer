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

## Usage

``` ruby
PhpSerializer.serialize('example')
PhpSerializer.unserialize('s:7:"example";')
```
## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/vijayrsv/php_serializer.

## License

The gem is available as open source under the terms of the [MIT License](http://opensource.org/licenses/MIT).

# coding: utf-8
lib = File.expand_path("../lib", __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require "php_serializer/version"

Gem::Specification.new do |spec|
  spec.name          = "php_serializer"
  spec.version       = PhpSerializer::VERSION
  spec.authors       = ["vijayrsv"]
  spec.email         = ["vijayrepomailzbox@gmail.com"]

  spec.summary       = %q{}
  spec.description   = %q{Native PHP serializer and unserializer(Note: currently only supports PHP primitive data-types) for Ruby and it is heavily inspired by PHP source code.}
  spec.homepage      = "https://github.com/vijayrsv/php_serializer"
  spec.license       = "MIT"
  spec.required_ruby_version = ">= 1.9.3"

  # Prevent pushing this gem to RubyGems.org. To allow pushes either set the 'allowed_push_host'
  # to allow pushing to a single host or delete this section to allow pushing to any host.
  if spec.respond_to?(:metadata)
    spec.metadata["allowed_push_host"] = "https://rubygems.org"
  else
    raise "RubyGems 2.0 or newer is required to protect against " \
      "public gem pushes."
  end

  spec.files         = `git ls-files -z`.split("\x0").reject do |f|
    f.match(%r{^(test|spec|features)/})
  end
  spec.bindir        = "exe"
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
	spec.extensions = %w[ext/php_serializer/extconf.rb]
  spec.require_paths = ["lib"]

  spec.add_development_dependency "bundler", "~> 1.15"
  spec.add_development_dependency "rake", "~> 10.0"
end

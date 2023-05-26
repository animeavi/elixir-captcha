priv/captcha: clean priv
	cargo build --release
	cp target/release/captcha priv/captcha

clean:
	rm -f priv/captcha

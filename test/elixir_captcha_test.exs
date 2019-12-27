defmodule CaptchaTest do
  use ExUnit.Case
  doctest Captcha

  test "successfully generates an image" do
    assert {:ok, _, _} = Captcha.get()
  end
end

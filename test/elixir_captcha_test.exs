defmodule CaptchaTest do
  use ExUnit.Case
  doctest Captcha

  test "successfully generates an image" do
    assert {:ok, _, _} = Captcha.get()
  end

  test "respects the timeout" do
    assert {:error, :timeout} = Captcha.get(0)
  end
end

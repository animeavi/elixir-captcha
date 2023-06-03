defmodule Captcha do
  def get() do
    case System.cmd(Application.app_dir(:captcha, "priv/captcha"), []) do
      {<<text::bytes-size(6), img::binary>>, 0} -> {:ok, text, img}
      _other -> :error
    end
  end
end
